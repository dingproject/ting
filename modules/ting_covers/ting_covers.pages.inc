<?php

/**
 * Return the URL for the cover of an object
 */
function ting_covers_object($preset, $object_id) {
	module_load_include('client.inc', 'ting');

  // $path is without the file_directory_path() prefix.
  $path = ting_covers_object_path($object_id);

  if (!ting_covers_cache_is_valid($path, $preset)) {
    $object = ting_get_object_by_id($object_id, true);

    if (!$object) {
      cache_set('ting_covers:' . $path, TRUE, 'cache', $_SERVER['REQUEST_TIME'] + TING_COVERS_CACHE_LIFETIME);
      ting_covers_goto_default($preset);
    }

    if ($object->additionalInformation->detailUrl) {
      $source_url = $object->additionalInformation->detailUrl;
    }
    elseif ($object->additionalInformation->thumbnailUrl) {
      $source_url = $object->additionalInformation->thumbnailUrl;
    }
    else {
      cache_set('ting_covers:' . $path, TRUE, 'cache', $_SERVER['REQUEST_TIME'] + TING_COVERS_CACHE_LIFETIME);
      ting_covers_goto_default($preset);
    }

    $path = _ting_covers_pages_fetch_or_redirect($path, $source_url, $preset);
  }

  drupal_goto(imagecache_create_url($preset, $path));
}

/**
 * Return the URL for the cover for a collection
 */
function ting_covers_collection($preset, $object_id) {
	module_load_include('client.inc', 'ting');

  //$path is without the file_directory_path() prefix.
  $path = ting_covers_collection_path($object_id);

  if (!ting_covers_cache_is_valid($path, $preset)) {
    $collection = ting_get_collection_by_id($object_id, true);
    if (!$collection) {
      cache_set('ting_covers:' . $path, TRUE, 'cache', $_SERVER['REQUEST_TIME'] + TING_COVERS_CACHE_LIFETIME);
      ting_covers_goto_default($preset);
    }

    //go through the objects in the collection looking for the first object with a cover
    $source_url = false;
    foreach ($collection->objects as $object) {
	    if ($object->additionalInformation->detailUrl) {
	      $source_url = $object->additionalInformation->detailUrl;
	      break; //we have a full high resolution image so move along!
	    }
	    elseif ($object->additionalInformation->thumbnailUrl) {
	      $source_url =$object->additionalInformation->thumbnailUrl;
	    }
    }

    $path = _ting_covers_pages_fetch_or_redirect($path, $source_url, $preset);
  }

  drupal_goto(imagecache_create_url($preset, $path));
}

/**
 * Return the URL for the cover of a FAUST number.
 */
function ting_covers_faust($preset, $faust_number) {
	module_load_include('client.inc', 'ting');

  //$path is without the file_directory_path() prefix.
  $path = ting_covers_faust_path($faust_number);

  if (!ting_covers_cache_is_valid($path, $preset)) {
    $info = ting_get_additional_info(array($faust_number));

    if (is_object($info[$faust_number])) {
      if ($info[$faust_number]->detailUrl) {
        $source_url = $info[$faust_number]->detailUrl;
      }
      elseif ($info[$faust_number]->thumbnailUrl) {
        $source_url = $info[$faust_number]->thumbnailUrl;
      }
    }

    $path = _ting_covers_pages_fetch_or_redirect($path, $source_url, $preset);
  }

  return drupal_goto(imagecache_create_url($preset, $path));
}

/**
 * Helper function to fetch image or redirect.
 *
 * @param string $path
 *    Path to save fetched image to.
 * @param string $source_url
 *    URL to fetch image from.
 * @param string $preset
 *    Name of imagecache preset to use for redirect.
 * @return string
 *    Image path if user was not redirected.
 */
function _ting_covers_pages_fetch_or_redirect($path, $source_url, $preset) {
  if ($source_url && filter_var($source_url, FILTER_VALIDATE_URL, FILTER_FLAG_SCHEME_REQUIRED)) {
    $path = _ting_covers_pages_fetch_image($path, $source_url);

    if ($path) {
      return $path;
    }
  }

  cache_set('ting_covers:' . $path, TRUE, 'cache', $_SERVER['REQUEST_TIME'] + TING_COVERS_CACHE_LIFETIME);
  ting_covers_goto_default($preset);
}

/**
 * Helper function to fetch and save image file from addi.dk.
 *
 * @see imagecache_build_derivative()
 *
 * @param string $filename
 *    File name, including its path within Drupal's file folder.
 * @param string $image_url
 *    URL for the source image file.
 * @return mixed
 *    A string containing the resulting filename or 0 on error.
 */
function _ting_covers_pages_fetch_image($filename, $image_url) {
  $result = drupal_http_request($image_url);

  if ($result->code == 200) {
    // get the folder for the final location of this preset...
    $dir = file_directory_path() . '/' . dirname($filename);

    // Build the destination folder tree if it doesn't already exists.
    if (!file_check_directory($dir, FILE_CREATE_DIRECTORY) &&
        !mkdir($dir, 0775, TRUE)) {
      return 0;
    }

    return file_save_data($result->data, $filename, FILE_EXISTS_REPLACE);
  }
  return 0;
}

