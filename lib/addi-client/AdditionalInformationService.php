<?php
/**
 * @file
 * AdditionalInformationService class.
 */

class AdditionalInformationService {

  protected $wsdlUrl;
  protected $username;
  protected $group;
  protected $password;
  protected $legacyMode = FALSE;

  /**
   * Instantiate the addi client.
   */
  public function __construct($wsdl_url, $username, $group, $password) {
    $this->wsdlUrl = $wsdl_url;
    $this->username = $username;
    $this->group = $group;
    $this->password = $password;

    // If we're speaking to the old addi web service, enable legacy mode.
    if (!strpos($this->wsdlUrl, 'moreinfo.addi.dk')) {
      $this->legacyMode = TRUE;
    }
  }

  /**
   * Get information by ISBN.
   *
   * @param mixed $isbn
   *   Expects either a single ISBN, or an array of them, for
   *   looking up multiple materials at a time.
   *
   * @return array
   *   Array of the images that were found.
   */
  public function getByIsbn($isbn) {
    $isbn = str_replace('-', '', $isbn);

    $identifiers = $this->collectIdentifiers('isbn', $isbn);
    $response = $this->sendRequest($identifiers);
    return $this->extractAdditionalInformation('isbn', $response);
  }

  /**
   * Get information by FAUST number.
   *
   * @param mixed $faust_number
   *   Expects either a single FAUST number, or an array of them, for
   *   looking up multiple materials at a time.
   *
   * @return array
   *   Array of the images that were found.
   */
  public function getByFaustNumber($faust_number) {
    $identifiers = $this->collectIdentifiers('faust', $faust_number);
    $response = $this->sendRequest($identifiers);
    return $this->extractAdditionalInformation('faust', $response);
  }

  /**
   * Get information by local ID and library code.
   *
   * @param mixed $local_id
   *   Expects either a single object with localIdentifier and
   *   libraryCode attributes, or an array of such objects.
   *
   * @return array
   *   Array of the images that were found.
   */
  public function getByLocalIdentifier($local_id) {
    $identifiers = $this->collectIdentifiers('localIdentifier', $local_id);
    $response = $this->sendRequest($identifiers);
    return $this->extractAdditionalInformation('localIdentifier', $response);
  }

  /**
   * Expand the provided IDs into the array structure used in sendRequest.
   */
  protected function collectIdentifiers($id_type, $ids) {
    if (!is_array($ids)) {
      $ids = array($ids);
    }

    $identifiers = array();
    foreach ($ids as $id) {
      // If we're passed objects from getByLocalIdentifier, convert them
      // to arrays.
      if (is_object($id)) {
        $identifiers[] = (array) $id;
      }
      // Otherwise, just map the ID type to the ID number.
      else {
        $identifiers[] = array($id_type => $id);
      }
    }

    return $identifiers;
  }

  /**
   * Send request to the addi server, returning the data response.
   */
  protected function sendRequest($identifiers) {
    $auth_info = array(
      'authenticationUser' => $this->username,
      'authenticationGroup' => $this->group,
      'authenticationPassword' => $this->password,
    );

    // New moreinfo service.
    $client = new SoapClient($this->wsdlUrl . '/moreinfo.wsdl');

    // Record the start time, so we can calculate the difference, once
    // the addi service responds.
    $start_time = explode(' ', microtime());

    $response = $client->moreInfo(array(
      'authentication' => $auth_info,
      'identifier' => $identifiers,
    ));

    // Calculate the request time.
    $stop_time = explode(' ', microtime());
    $time = floatval(($stop_time[1] + $stop_time[0]) - ($start_time[1] + $start_time[0]));

    // Drupal specific code - consider moving this elsewhere.
    if (variable_get('addi_enable_logging', FALSE)) {
      watchdog('addi', 'Completed request in @seconds seconds.', array(
        '@seconds' => $time,
      ), WATCHDOG_DEBUG, 'http://' . $_SERVER["HTTP_HOST"] . $_SERVER["REQUEST_URI"]);
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
    if (!$this->legacyMode) {
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
