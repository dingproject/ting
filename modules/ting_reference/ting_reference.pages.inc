<?php

/**
 * JSON callback to Ting reference collection autocomplete
 */
function ting_reference_collection_autocomplete_js() {
	module_load_include('client.inc', 'ting');
	
	$string = $_REQUEST['query'];
	
	$items = array();
	if ($string && (strlen($string) > 0))
	{
		$searchResult = ting_do_search($string, 1, 10, array('facets' => array()));
		if (isset($searchResult->collections) && is_array($searchResult->collections)) {
	    foreach ($searchResult->collections as $collection) {
	      $items[$collection->id] = t('!author: !title (!manifestations)', array(
	        '!author' => $collection->creators_string,
	        '!title' => $collection->title,
	        '!manifestations' => format_plural(count($collection->objects), '1 manifestation', '@count manifestations'),
	      ));
	    }
	  }
	}
	
	echo ting_reference_autocomplete_format_items($items);
	exit;
}

/**
 * JSON callback to Ting reference collection autocomplete
 */
function ting_reference_object_autocomplete_js() {
	module_load_include('client.inc', 'ting');

	$string = $_REQUEST['query'];
	
	$items = array();
	if ($string && (strlen($string) > 0)) {
		$searchResult = ting_do_search($string, 1, 10, array('useCollections' => 'false', 'facets' => array()));
		
    // If there is no usable result, exit immediately, providng no reply.
    if (!$searchResult) { exit; }

		foreach ($searchResult->collections as $collection) {
			$object = array_shift($collection->objects);
      $items[$object->id] = t('!author: !title (!type, !year, !id)',  array(
        '!author' => $object->creators[0],
        '!title' => $object->title,
        '!type' => $object->type,
        '!year' => $object->record['dc:date'][''][0],
        '!id' => $object->localId,
      ));
		}
	}

	echo ting_reference_autocomplete_format_items($items);
	exit;
}

function ting_reference_autocomplete_format_items($items) {
  $return = array();
  foreach ($items as $id => $value) {
    // jQuery Autocomplete uses pipe (0+007C) and does not provide a way to
    // escape this character, so replace it with similar-looking character
    // (U+00A6).
    $return[] = str_replace('|', '¦', $value).'|'.$id;
  }
  return implode("\n", $return);
}

/**
 * JSON callback Ting reference preview on node edit page.
 */
function ting_reference_collection_js($ref_type, $object_id) {
  $collection = _ting_reference_get_collection($ref_type, $object_id);
  $output = theme('ting_reference_preview', $collection);
  drupal_json($output);
}
