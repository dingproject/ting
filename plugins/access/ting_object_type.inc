<?php

/**
 * @file
 * Plugin to provide access control based upon a library node's status 
 * as a special library.
 */

/**
 * Implementation of hook_ctools_access().
 */
function ting_ting_object_type_ctools_access() {
  $items = array();
  $items['ting_object_type'] = array(
    'title' => t('Ting object type'),
    'description' => t('Control access by Ting object type.'),
    'callback' => 'ting_ting_object_type_type_ctools_access_check',
    'default' => array('ting_object_type' => array()),
    'settings form' => 'ting_ting_object_type_type_ctools_access_settings',
    'summary' => 'ting_ting_object_type_type_ctools_access_summary',
    'required context' => new ctools_context_required(t('Ting object'), 'ting_object'),
  );
  return $items;
}

/**
 * Check for access.
 */
function ting_ting_object_type_type_ctools_access_check($conf, $context) {
  // As far as I know there should always be a context at this point,
  // but this is safe.
  if (empty($context) || empty($context->data) || !isset($context->data->type)) {
    return FALSE;
  }

  // Permit access of object has one of the allowed types.
  if (in_array($context->data->type, $conf['ting_object_type'])) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Add the type to the settings form.
 */
function ting_ting_object_type_type_ctools_access_settings(&$form, &$form_state, $conf) {
  $form['settings']['ting_object_type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Ting object type'),
    '#options' => array(
      // TODO: Expand this list. Can we get it from somewhere dynamically?
      'Tidsskrift' => t('Periodical magazine'),
      'Periodikum' => t('Periodical'),
      'Årbog' => t('Yearbook'),
    ),
    '#default_value' => (isset($conf['ting_object_type'])) ? $conf['ting_object_type'] : array(),
    '#description' => t('Pick the allowed object types.'),
  );
}

/**
 * Provide a summary description of what we checked.
 */
function ting_ting_object_type_type_ctools_access_summary($conf, $context) {
  $type = (isset($conf['ting_object_type'])) ? $conf['ting_object_type'] : array();
  return t('@identifier must be @type', array('@identifier' => $context->identifier, '@type' => implode(', ', $type)));
}

