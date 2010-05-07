<?php
// $Id$

/**
 * @file ting_collection.inc
 *
 * Plugin to provide a Ting collection context.
 */

/**
 * Implementation of specially named hook_ctools_contexts().
 */
function ting_ting_collection_ctools_contexts() {
  $args = array();
  $args['ting_collection'] = array(
    'title' => t("Ting collection"),
    'description' => t('An collection of objects from the Ting database.'),
    'context' => 'ting_ting_collection_create',
    'settings form' => 'ting_ting_collection_context_settings_form',
    'keyword' => 'ting_collection',
    'context name' => 'ting_collection',
    'convert list' => 'ting_ting_collection_convert_list',
    'convert' => 'ting_ting_collection_convert', 
    'placeholder form' => array(
      '#type' => 'textfield',
      '#description' => t('Enter the collection ID for this context'),
    ),
  );
  return $args;
}

/**
 * It's important to remember that $conf is optional here, because contexts
 * are not always created from the UI.
 */
function ting_ting_collection_create($empty, $data = NULL, $conf = FALSE) {
  $context = new ctools_context('ting_collection');
  $context->plugin = 'ting_collection';

  if ($empty) {
    return $context;
  }

  if ($conf) {
    $collection_id = is_array($data) && isset($data['object_id']) ? $data['object_id'] : (is_object($data) ? $data->id : 0);

    module_load_include('client.inc', 'ting');
    $data = ting_get_collection_by_id($collection_id);
  }

  if (!empty($data)) {
    $context->data     = $data;
    $context->title    = t('@title by @author', array('@title' => $data->title, '@author' => $data->creators_string));
    $context->argument = $collection_id;

    return $context;
  }
}

/**
 * Settings for when manually creating this context.
 */
function ting_ting_collection_context_settings_form($conf) {
  $form = array();
  return $form;
}

/**
 * List of conversion options.
 */
function ting_ting_collection_convert_list() {
  return array(
    'title_full' => t('Collection title and author names'),
    //'title' => t('Collection title'),
  );
}

/**
 * Convert a context into a string.
 */
function ting_ting_collection_convert($context, $type) {
  switch ($type) {
    case 'title_full':
      return $context->title;
  }
}

