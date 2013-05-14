<?php
/**
 * @file
 * Admin pages for Ting module.
 */

/**
 * Ting search settings form.
 */
function ting_admin_settings_form($form_state) {
  $form = array();

  $form['ting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ting service settings'),
    '#tree' => FALSE,
  );
  $form['ting']['ting_agency'] = array(
    '#type' => 'textfield',
    '#title' => t('Library code'),
    '#description' => t('The 6-digit code representing the library organization'),
    '#default_value' => variable_get('ting_agency', ''),
  );
  $form['ting']['ting_profile'] = array(
    '#type' => 'textfield',
    '#title' => t('Sources profile'),
    '#description' => t('The name of the sources profile to be used when searching. Default is opac.
    										 Note that the name must be exactly as defined in !vip_link', array('!vip_link' => l(t('DBCs VIP database'), 'http://vip.dbc.dk'))),
    '#default_value' => variable_get('ting_profile', 'opac'),
  );

  $form['ting']['ting_search_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Search service URL'),
    '#description' => t('URL to the Ting search webservice, e.g. http://opensearch.addi.dk/2.0/'),
    '#required' => TRUE,
    '#default_value' => variable_get('ting_search_url', ''),
  );
  $form['ting']['ting_scan_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Scan service URL'),
    '#description' => t('URL to the Ting scan webservice, e.g. http://openscan.addi.dk/2.0/'),
    '#required' => TRUE,
    '#default_value' => variable_get('ting_scan_url', ''),
  );
  $form['ting']['ting_spell_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Spell service URL'),
    '#description' => t('URL to the Ting spell webservice, e.g. http://openspell.addi.dk/1.2/'),
    '#required' => TRUE,
    '#default_value' => variable_get('ting_spell_url', ''),
  );
  $form['ting']['ting_recommendation_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Recommendation service URL'),
    '#description' => t('URL to the Ting recommendation webservice (Andre der har lånt...) , e.g. http://openadhl.addi.dk/1.1/'),
    '#required' => TRUE,
    '#default_value' => variable_get('ting_recommendation_server', ''),
  );
  $form['ting']['ting_enable_logging'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable logging'),
    '#default_value' => variable_get('ting_enable_logging', false),
  );

  $form['addi'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Information settings'),
  	'#description' => t('The Additional Information service is used to retrieve cover images and other information about objects. <a href="http://www.danbib.dk/index.php?doc=forsideservice">More information about the service (in Danish)</a>'),
    '#tree' => FALSE,
  );
  $form['addi']['addi_wsdl_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Service URL'),
    '#description' => t('URL to the Additional Information webservice, e.g. http://forside.addi.dk/addi.wsdl'),
    '#required' => TRUE,
    '#default_value' => variable_get('addi_wsdl_url', ''),
  );
  $form['addi']['addi_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('Username for the Additional information webservice'),
    '#required' => TRUE,
    '#default_value' => variable_get('addi_username', ''),
  );
  $form['addi']['addi_group'] = array(
    '#type' => 'textfield',
    '#title' => t('Group'),
    '#description' => t('User group for the Additional information webservice'),
    '#required' => TRUE,
    '#default_value' => variable_get('addi_group', ''),
  );
  $form['addi']['addi_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#description' => t('Password for the Additional information webservice'),
    '#required' => TRUE,
    '#default_value' => variable_get('addi_password', ''),
  );
  $form['addi']['addi_enable_logging'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable logging'),
    '#default_value' => variable_get('addi_enable_logging', false),
  );

  return system_settings_form($form);
}

/**
 * Value callback for restricted_access_proxy_hostnames field defined in
 * ting_admin_settings_form().
 */
function ting_admin_settings_hostnames_value($form, $edit = FALSE) {
  if ($edit !== FALSE) {
    return array_filter(array_map('trim', explode("\n", $edit)));
  }
}

/**
 * Ranking settings form.
 */
function ting_admin_ranking_form(&$form_state) {
  $form = array(
    '#after_build' => array('ting_admin_ranking_form_after_build'),
  );

  $form['ting_ranking_custom'] = array(
    '#title' => t('Enable custom ranking'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('ting_ranking_custom', 0),
    '#description' => t('If enabled, the Ting client will use the custom ranking settings provided below when making queries to Ting.'),
  );

  // Get the stored ranking fields.
  $fields = variable_get('ting_ranking_fields', array());

  // Wrapper, so that the ahah callback have some place to put new elements
  $form['ting_ranking_fields'] = array(
    '#prefix' => '<h4>' . t('Custom ranking fields') . '</h4><div id="ting_ranking_fields">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  // If there is no pre-existing fields, create an empty one.
  if (empty($fields)) {
    $form['ting_ranking_fields'][0] = array(
      '#type' => 'ting_ranking_field',
      '#default_value' => array(),
    );
  }
  else {
    foreach ($fields as $delta => $field) {
      $form['ting_ranking_fields'][$delta] = array(
        '#type' => 'ting_ranking_field',
        '#default_value' => $field,
      );
    }
  }

  $form['add_ranking_field'] = array(
    '#type' => 'submit',
    '#value' => t('Add another ranking field'),
    '#weight' => 1,
    '#ahah' => array(
      'path' => 'ting/ranking_field/ahah',
      'wrapper' => 'ting_ranking_fields',
      'method' => 'append',
      'effect' => 'slide'
    ),
  );

  $form['buttons']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
  );

  return $form;
}

/**
 * Submit handler for ranking settings form.
 */
function ting_admin_ranking_form_submit($form, &$form_state) {
  variable_set('ting_ranking_custom', (integer) $form_state['values']['ting_ranking_custom']);

  $fields = $form_state['values']['ting_ranking_fields'];

  $fields = array_filter($fields, '_ting_ranking_field_filter');
  usort($fields, '_ting_ranking_field_sort');

  variable_set('ting_ranking_fields', $fields);
}

/**
 * After build callback to the ranking settings form.
 *
 * Add a little bit of CSS and JS to the form.
 */
function ting_admin_ranking_form_after_build($form, &$form_state) {
  $path = drupal_get_path('module', 'ting');

  drupal_add_css($path .'/css/ting_admin_ranking_form.css');
  drupal_add_js($path . '/js/ting_admin_ranking_form.js');

  return $form;
}

/**
 * array_filter() callback to remove empty/deleted elements.
 */
function _ting_ranking_field_filter($element) {
  return !empty($element['field_name']);
}

/**
 * usort() callback to remove empty/deleted elements.
 */
function _ting_ranking_field_sort($a, $b) {
  if ($a['weight'] == $b['weight']) {
      return 0;
  }
  return ($a['weight'] > $b['weight']) ? -1 : 1;
}

/**
 * AHAH callback to add another ranking field to the form.
 */
function ting_admin_ranking_add_field_ahah() {
  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];

  // Drupal is not aware of this new elements existence and will not
  // process it. We retreive the cached form, add the element, and resave.
  $form = form_get_cache($form_build_id, $form_state);

  // Add one to the current highest delta value to get the next one.
  $delta = max(element_children($form['ting_ranking_fields'])) + 1;

  // Added field and update form cache
  $form['ting_ranking_fields'][$delta] = array(
    '#type' => 'ting_ranking_field',
    '#default_value' => array(),
  );
  form_set_cache($form_build_id, $form, $form_state);

  // Reset form vars
  $form += array(
    '#post' => $_POST,
    '#programmed' => FALSE,
  );

  // Rebuild the form.
  $form = form_builder($_POST['form_id'], $form, $form_state);

  // Render and return the new field.
  drupal_json(array(
    'data' => drupal_render($form['ting_ranking_fields'][$delta]),
    'status' => TRUE));
  exit();
}

/**
 * Form builder; Configure custom search result ranking for this site.
 *
 * @ingroup forms
 */
function ting_admin_boost_settings($form, &$form_state) {
  $form = array('#after_build' => array('ting_admin_boost_form_after_build'));

  // Get our stored data.
  $field_data = variable_get('ting_boost_fields', array());

  if (!isset($form_state['boost_field_count'])) {
    $form_state['boost_field_count'] = count($field_data);
  }

  // Wrapper, so that the AJAX callback have some place to put new elements
  $form['ting_boost_fields'] = array(
    '#title' => t('Custom fields boost values'),
    '#type' => 'fieldset',
    '#prefix' => '<div id="boost-fields-wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  foreach (range(0, $form_state['boost_field_count']) as $i) {
    $form['ting_boost_fields']['fields'][$i] = array(
      '#type' => 'ting_boost_field',
      '#default_value' => (isset($field_data[$i])) ? $field_data[$i] : array(),
    );
  }

  $form['ting_boost_fields']['add_value'] = array(
    '#type' => 'submit',
    '#value' => t('Add another boost value'),
    '#submit' => array('ting_admin_boost_add_more_submit'),
    '#weight' => 10,
    '#ahah' => array(
      'path' => 'ting/boost_field/ahah',
      'wrapper' => 'boost-fields-wrapper',
      'method' => 'append',
      'effect' => 'slide'
    ),
  );

  $form['buttons']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
  );

  return $form;
}

/**
 * Submit handler for the "Add another item" button of a field form.
 *
 * This handler is run regardless of whether JS is enabled or not. It makes
 * changes to the form state. If the button was clicked with JS disabled, then
 * the page is reloaded with the complete rebuilt form. If the button was
 * clicked with JS enabled, then ajax_form_callback() calls field_add_more_js()
 * to return just the changed part of the form.
 */
function ting_admin_boost_add_more_submit($form, &$form_state) {
  // Simply add another empty set of values to the field array and ask
  // to have the form rebuilt. The form code will take care of the rest.
  $form_state['boost_field_count'] += 1;
  $form_state['rebuild'] = TRUE;
}

/**
 * AHAH callback to add another boost field to the form.
 */
function ting_admin_boost_add_field_ahah() {
  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];

  // Drupal is not aware of this new elements existence and will not
  // process it. We retreive the cached form, add the element, and resave.
  $form = form_get_cache($form_build_id, $form_state);

  // Add one to the current highest delta value to get the next one.
  $delta = max(element_children($form['ting_boost_fields']['fields'])) + 1;

  // Added field and update form cache
  $form['ting_boost_fields']['fields'][$delta] = array(
    '#type' => 'ting_boost_field',
    '#default_value' => array(),
  );
  form_set_cache($form_build_id, $form, $form_state);

  // Reset form vars
  $form += array(
    '#post' => $_POST,
    '#programmed' => FALSE,
  );

  // Rebuild the form.
  $form = form_builder($_POST['form_id'], $form, $form_state);

  // Render and return the new field.
  drupal_json(array(
    'data' => drupal_render($form['ting_boost_fields']['fields'][$delta]),
    'status' => TRUE));
  exit();
}

/**
 * After build callback to the boost settings form.
 *
 * Add a little bit of CSS and JS to the form.
 */
function ting_admin_boost_form_after_build($form, &$form_state) {
  $path = drupal_get_path('module', 'ting');

  drupal_add_css($path .'/css/ting_admin_boost_form.css');

  return $form;
}

/**
 * Submit handler for boost settings form.
 */
function ting_admin_boost_settings_submit($form, &$form_state) {
  $fields = $form_state['values']['ting_boost_fields']['fields'];

  $fields = array_filter($fields, '_ting_boost_field_filter');
  usort($fields, '_ting_boost_field_sort');

  variable_set('ting_boost_fields', $fields);
}

/**
 * array_filter() callback to remove empty/deleted elements.
 */
function _ting_boost_field_filter($element) {
  return !(empty($element['field_name']) || empty($element['field_value']));
}

/**
 * usort() callback to remove empty/deleted elements.
 */
function _ting_boost_field_sort($a, $b) {
  if ($a['weight'] == $b['weight']) {
      return 0;
  }
  return ($a['weight'] > $b['weight']) ? -1 : 1;
}

