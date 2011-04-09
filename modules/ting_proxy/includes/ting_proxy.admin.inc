<?php

/**
 * Administration UI for ting proxy backend
 *
 * @param array $form_state
 * @return array form
 */
function ting_proxy_settings_form($form_state) {
  // Load saved settings
  $values = variable_get('ting_proxy', array());

  // Build the form
  $form = array();
  $form['#after_build'] = array('ting_proxy_settings_form_after_build');

  $form['ting_proxy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Proxy service'),
    '#required' => TRUE,
    '#description' => t('Restricted-access resources referenced by Ting may be accessed through the library\'s proxy server'),
    '#tree' => TRUE,
  );

  $form['ting_proxy']['prefix'] = array(
    '#type' => 'textfield',
    '#title' => 'Proxy server URL prefix',
    '#default_value' => isset($values['prefix']) ? $values['prefix'] : '',
    '#description' => t('E.g. <em>http://proxy.example.com/?url=</em>.'),
  );

  // Wrapper, so that the ahah callback have some place to put new elements
  $form['ting_proxy']['hostnames'] = array(
    '#prefix' => '<div id="hostnames">',
    '#suffix' => '</div>',
  );

  // Build hostnames
  if (isset($values['hostnames']) && empty($values['hostnames'])) {
    $form['ting_proxy']['hostnames'][0] = array(
      '#type' => 'ting_proxy_hostname',
      '#default_value' => '',
    );
  }
  else {
    $i = 0;
    foreach ($values['hostnames'] as $value) {
      $form['ting_proxy']['hostnames'][$i] = array(
        '#type' => 'ting_proxy_hostname',
        '#default_value' => $value,
      );
      $i++;
    }
  }

  // Add ahah button
  $form['ting_proxy']['add_hostname'] = array(
    '#type' => 'submit',
    '#value' => t('Add another hostname'),
    '#weight' => 1,
    //'#submit' => array('ting_proxy_add_hostname_submit'),
    '#ahah' => array(
      'path' => 'ting/proxy/hostname/ahah',
      'wrapper' => 'hostnames',
      'method' => 'append',
      'effect' => 'slide'
    ),
  );
  
  // Get systemet form elements
  $form = system_settings_form($form);

  // Override submit as we need to remove elements marked deleted
  $form['#submit'] = array('ting_proxy_settings_form_submit');

  // The final form
  return $form;
}

/**
 * Submit handler for the administration forms. It preformces basic clean up
 * and hands the form to systemt settings submit.
 *
 * @param array $form
 * @param array $form_state
 */
function ting_proxy_settings_form_submit($form, &$form_state) {
  // Remove add hostname button
  unset($form_state['values']['ting_proxy']['hostnames']['add_hostname']);

  // Remove elements that have been marked as deleted
  foreach ($form_state['values']['ting_proxy']['hostnames'] as $key => $value) {
    if ($value['deleted']) {
      unset($form_state['values']['ting_proxy']['hostnames'][$key]);
    }
  }

  // Return control to system settings form submit.
  system_settings_form_submit($form, $form_state);
}

/**
 * Handels addition of extra ting_proxy_hostname elements via ahah to the
 * adminitration UI.
 *
 * @param array $form
 * @param array $form_state
 */
function ting_proxy_add_hostname_ahah($form, &$form_state) {

  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];

  // Drupal is not aware of this new elements existence and will not
  // process it. We retreive the cached form, add the element, and resave.
  $form = form_get_cache($form_build_id, $form_state);

  // Delta is assigned to the count of current elements, so it'll be the
  // next in the zero-based list.
  $delta = count(element_children($form['ting_proxy']['hostnames']));
  $delta++;

  // Added field and update form cache
  $form['ting_proxy']['hostnames'][$delta] = array(
    '#type' => 'ting_proxy_hostname',
    '#default_value' => '',
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
    'data' => drupal_render($form['ting_proxy']['hostnames'][$delta]),
    'status' => TRUE));
  exit();
}

/**
 * Addes custom ccs and js script to the administration form. It's done this way
 * as it's add to the forms validation as well.
 *
 * @param array $form
 * @param array $form_state
 * @return array $form
 */
function ting_proxy_settings_form_after_build($form, &$form_state) {
  // Add form styling
  drupal_add_css(drupal_get_path('module', 'ting_proxy') .'/css/ting_proxy.css', 'module', 'all', FALSE);

  // Add js
  drupal_add_js(drupal_get_path('module', 'ting_proxy'). '/js/ting_proxy.admin.js');

  return $form;
}
