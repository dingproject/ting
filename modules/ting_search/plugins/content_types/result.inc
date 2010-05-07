<?php
// $Id$

/**
 * @file result.inc
 * Provides search results as a Panels content type.
 */

/**
 * Callback function to supply a list of content types.
 *
 * This is only required to make CTools know that we're providing 
 * multiple types here (since we have no single => TRUE in the array).
 */
function ting_search_result_ctools_content_types() {
  return array(
    'title' => t('Ting search results'),
    'single' => TRUE,
    'description' => t('The search results for a search against the Ting system'),
    'required context' => new ctools_context_required(t('Search keys'), 'string'),
    'category' => t('Ting'),
  );
}

/**
 * Return the pane contents.
 */
function ting_search_result_content_type_render($subtype, $conf, $panel_args, $context) {
  $block = new stdClass();

  $keys = trim($context->data);
  if (strlen($keys) > 0) {
    // Add links to feeds.
    drupal_set_html_head(
      theme('atom_feed_link', t('Search feed for “@keys”', array('@keys' => $keys)), url('ting/search/feed/' . $keys)) .
      theme('atom_feed_link', t('Content search feed for “@keys”', array('@keys' => $keys)), url('ting/search/content/feed/' . $keys))
    );

    // Loads the JavaScript files.
    ting_search_add_js();

    drupal_add_js(array('tingSearch' => array(
      'keys' => $keys,
      'ting_url' => url('ting/search/js'),
      'content_url' => url('ting/search/content/js'),
      'result_template' => theme('ting_result')
    )), 'setting', 'header');
    $block->content = theme('ting_search_result_page', $keys);
  }
  return $block;
}

/**
 * Preprocess hook for ting_result.
 */
function template_preprocess_ting_result(&$vars) {
  module_load_include('client.inc', 'ting');
  $vars['sort_options'] = ting_search_sort_options();
}

/**
 * Return the pane titles.
 */
function ting_search_result_content_type_admin_title($subtype, $conf, $context) {
  return t('“@s”: Ting search results', array('@s' => $context->identifier));
}

/**
 * Returns an edit form for the content type.
 *
 * We're not actually doing anything here, but we need to have this hook 
 * for this content type to work.
 */
function ting_search_result_content_type_edit_form(&$form, &$form_state) {
  return $form;
}

