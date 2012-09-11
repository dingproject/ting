<?php
/**
 * @file
 * AdditionalInformationService class.
 */

require_once dirname(__FILE__) . '/AdditionalInformationServiceException.php';
require_once dirname(__FILE__) . '/AdditionalInformation.php';

class AdditionalInformationService {

  protected $wsdlUrl;
  protected $username;
  protected $group;
  protected $password;

  /**
   * Instantiate the addi client.
   */
  public function __construct($wsdl_url, $username, $group, $password) {
    $this->wsdlUrl = $wsdl_url;
    $this->username = $username;
    $this->group = $group;
    $this->password = $password;
  }

  /**
   * Get information by ISBN.
   */
  public function getByIsbn($isbn) {
    $isbn = str_replace('-', '', $isbn);

    $identifiers = $this->collectIdentifiers('isbn', $isbn);
    $response = $this->sendRequest($identifiers);
    return $this->extractAdditionalInformation('isbn', $response);
  }

  /**
   * Get information by FAUST number.
   */
  public function getByFaustNumber($faust_number) {
    $identifiers = $this->collectIdentifiers('faust', $faust_number);
    $response = $this->sendRequest($identifiers);
    return $this->extractAdditionalInformation('faust', $response);
  }

  /**
   * Expand the provided IDs into the array structure used in sendRequest.
   */
  protected function collectIdentifiers($id_type, $ids) {
    if (!is_array($ids)) {
      $ids = array($ids);
    }

    $identifiers = array();
    foreach ($ids as $i) {
      $identifiers[] = array($id_type => $i);
    }

    return $identifiers;
  }

  /**
   * Send request to the addi server, returning the data response.
   */
  protected function sendRequest($identifiers) {
    $ids = array();

    foreach ($identifiers as $i) {
      $ids = array_merge($ids, array_values($i));
    }

    $auth_info = array(
      'authenticationUser' => $this->username,
      'authenticationGroup' => $this->group,
      'authenticationPassword' => $this->password,
    );

    if (preg_match('/moreinfo.addi.dk/', $this->wsdlUrl)) {
      // New moreinfo service.
      $client = new SoapClient($this->wsdlUrl . '/moreinfo.wsdl');
      $method = 'moreInfo';
    }
    else {
      // Legacy additionalInformation service.
      $client = new SoapClient($this->wsdlUrl);
      $method = 'additionalInformation';
    }

    $start_time = explode(' ', microtime());
    $response = $client->$method(array(
                          'authentication' => $auth_info,
                          'identifier' => $identifiers));
    $stop_time = explode(' ', microtime());
    $time = floatval(($stop_time[1] + $stop_time[0]) - ($start_time[1] + $start_time[0]));

    // Drupal specific code - consider moving this elsewhere.
    if (variable_get('addi_enable_logging', FALSE)) {
      watchdog('addi', 'Completed request (' . round($time, 3) . 's): Ids: %ids', array('%ids' => implode(', ', $ids)), WATCHDOG_DEBUG, 'http://' . $_SERVER["HTTP_HOST"] . $_SERVER["REQUEST_URI"]);
    }

    if ($response->requestStatus->statusEnum != 'ok') {
      throw new AdditionalInformationServiceException($response->requestStatus->statusEnum . ': ' . $response->requestStatus->errorText);
    }

    if (!is_array($response->identifierInformation)) {
      $response->identifierInformation = array($response->identifierInformation);
    }

    return $response;
  }

  /**
   * Extract the data we need from the server response.
   */
  protected function extractAdditionalInformation($id_type, $response) {
    if (preg_match('/moreinfo.addi.dk/', $this->wsdlUrl)) {
      // New moreinfo service.
      $image_prop = 'coverImage';
    }
    else {
      // Legacy additionalInformation service.
      $image_prop = 'image';
    }

    // We get a response for each ID we inquired on.
    $responses = array();

    foreach ($response->identifierInformation as $info) {
      $thumbnail_url = $detail_url = NULL;
      if (isset($info->identifierKnown) &&
          $info->identifierKnown) {
        if (!is_array($info->$image_prop)) {
          $info->$image_prop = array($info->$image_prop);
        }

        foreach ($info->$image_prop as $image) {
          switch ($image->imageSize) {
            case 'thumbnail':
              $thumbnail_url = $image->_;
              break;

            case 'detail':
              $detail_url = $image->_;
              break;

            default:
              // Do nothing other image sizes may appear but ignore them
              // for now.
          }
        }

        $ai_instance = new AdditionalInformation($thumbnail_url, $detail_url);
        $responses[$info->identifier->$id_type] = $ai_instance;
      }
    }

    return $responses;
  }
}
