<?php

/**
 * Search result page.
 */
function ting_search_pages_redirect() {
  if (isset($_POST['keys'])) {
    return drupal_goto('ting/search/' . $_POST['keys']);
  }

  $keys = search_get_keys();
  if (strlen($keys) > 0) {
    // Redirect to the new Panels based search.
    return drupal_goto('ting/search/' . $keys, NULL, NULL, 301);
  }
  return drupal_not_found();
}

/**
 * JSON callback to search Ting.
 */
function ting_search_pages_ting_js() {
  module_load_include('client.inc', 'ting');

  //Get users base query
  $query = $_REQUEST['query'];
  if (!$query) {
    return drupal_not_found();
  }

  //Wrap base query
  $query = '('.$query.')';

  //Extend query with selected facets
  if (isset($_GET['facets']) && $_GET['facets']) {
    $facetArray = array();

    $facets = explode(';', $_GET['facets']);
    foreach ($facets as $facet) {
      $facet = explode(':', $facet, 2);
      if ($facet[0]) {
        $facetArray[] = $facet[0].'="'.$facet[1].'"';
      }
    }

    $query .= ' AND '.implode(' AND ', $facetArray);
  }

  //Set sort method
  $options['sort'] = (isset($_GET['sort'])) ? $_GET['sort'] : NULL;

  try {
    $page = isset($_REQUEST['page']) ? intval($_REQUEST['page']) : 1;
    $resultsPerPage = 10;
    $searchResult = ting_do_search($query, $page, $resultsPerPage, $options);

    $result = array(
      'page' => $page,
      'resultsPerPage' => $resultsPerPage,
      'count' => (integer) $searchResult->numTotalObjects,
      'collectionCount' => (integer) $searchResult->numTotalCollections,
      'result_html' => '',
      'facets' => $searchResult->facets,
      'local_ids' => array(),
      'feed_icon' => theme('feed_icon', url('ting/search/feed/' . $_REQUEST['query'])),
    );

    if (isset($searchResult->collections)) {
      foreach ($searchResult->collections as &$collection) {
        $result['result_html'] .= theme('ting_search_collection', $collection);

        foreach ($collection->objects as $object) {
          $result['local_ids'][] = $object->localId;
        }
      }
    }
  }
  catch (TingClientException $e) {
    // TODO: Log the error.
    $result = array('count' => 0);
  }

  drupal_json($result);
  exit;
}

/**
 * JSON callback to Drupal's content search.
 */
function ting_search_pages_content_js() {
  $result = array(
    'count' => 0,
  );
  if (isset($_REQUEST['query'])) {
    // Drupal's do_search() is hard-coded to 10 results per page.
    $max_results = 10;
    // Identifier used by pager_query().
    $element = 0;
    $query = trim($_REQUEST['query']);

    // Stolen from node_search(). Needed to respect access and status.
    list($join1, $where1) = _db_rewrite_sql();
    $arguments1 = array();
    $conditions1 = 'n.status = 1';
    $search_result = do_search($query, 'node', 'INNER JOIN {node} n ON n.nid = i.sid' . $join1, $conditions1 . (empty($where1) ? '' : ' AND '. $where1), $arguments1);

    $result = array(
      // do_search() calls pager_query() that populates these globals.
      'count' => $search_result ? $GLOBALS['pager_total_items'][$element] : 0,
      // pager_query() page numbers are zero-based.
      'page' => $search_result ? $GLOBALS['pager_page_array'][$element] + 1 : 0,
      'resultsPerPage' => $max_results,
      'result_html' => '',
      'feed_icon' => theme('feed_icon', url('ting/search/content/feed/' . $_REQUEST['query'])),

    );
    foreach ($search_result as $item) {
      if ($item->type == 'node') {
        $node = node_load($item->sid, NULL, TRUE);
        $result['result_html'] .= node_view($node, TRUE);
      }
    }
    if ($search_result) {
      $result['result_html'] .= theme('pager', NULL, $max_results, $element);
    }
  }

  drupal_json($result);
  exit;
}

/**
 * Generate Atom feed of Ting search results.
 */
function ting_search_pages_ting_feed($query) {
  module_load_include('client.inc', 'ting');
  $entries = '';
  $max_results = 10;
  $result_count = 0;
  $cache_key = 'ting_search_ting_feed_entries:' . md5(drupal_strtolower($query));
  $cache_data = cache_get($cache_key);

  if ($cache_data) {
    $result_count = $cache_data->data['result_count'];
    $entries = $cache_data->data['entries'];
  }
  else {
    try {
      $search_result = ting_do_search($query, 1, 10);
      $result_count = $search_result->numTotalObjects;
      foreach ($search_result->collections as $collection) {
        $entry = array(
          'id' => url($collection->url, array('absolute' => TRUE)),
          'title' => $collection->title,
          'summary' => $collection->abstract,
          'authors' => array(),
          'categories' => array(),
        );

        if ($collection->date) {
          $entry['published'] = $collection->date . '-01-01T12:00:00Z';
        }

        // Since we don't have any content, we must provide an alternate link.
        $entry['alternate_link'] = $entry['id'];

        // We have no data available about when something was updated, so
        // we'll reuse the published attribute (since updated is a
        // required attribute per Atom spec).
        $entry['updated'] = $entry['published'];

        foreach ($collection->creators as $creator) {
          $entry['authors'][] = array('name' => $creator);
        }

        foreach ($collection->subjects as $subject) {
          $entry['categories'][] = array('term' => $subject);
        }

        $entries .= theme('atom_entry', $entry);
      }
      // Cache the fetched data for 8 hours.
      cache_set($cache_key, array('result_count' => $result_count, 'entries' => $entries), 'cache', time() + 28000);
    }
    catch (TingClientException $e) {
    }
  }

  $feed = array(
    'title' => t('“@title” search results feed', array('@title' => $query)),
    'alternate_link' => url('ting/search/' . $query, array('absolute' => TRUE)),
    'entries' => $entries,
  );

  // If data was received from cache, set the feeds update time to the
  // cache creation time.
  if ($cache_data) {
    $feed['updated'] = gmdate('c', $cache_data->created);
  }

  if ($result_count > $max_results) {
    $feed['subtitle'] = t('First @max of @count items found.', array('@max' => $max_results, '@count' => $result_count));
  }
  elseif ($result_count > 0) {
    $feed['subtitle'] = format_plural($result_count, 'One item found', '@count items found.', array('@count' => $result_count));
  }
  else {
    $feed['subtitle'] = t('No items found.');
  }

  // Set the correct content type for Atom feeds.
  drupal_set_header('Content-Type: application/atom+xml; charset=utf-8');
  print theme('atom_feed', $feed);
  return;
}

/**
 * Generate Atom feed of content search results.
 */
function ting_search_pages_content_feed($query) {
  $entries = '';
  $max_results = 10;
  $result_count = 0;
  $cache_key = 'ting_search_content_feed_entries:' . md5(drupal_strtolower($query));
  $cache_data = cache_get($cache_key);

  if ($cache_data) {
    $result_count = $cache_data->data['result_count'];
    $entries = $cache_data->data['entries'];
  }
  else {
    $search_result = do_search($query, 'node');
    // do_search() calls pager_query() that populates this global.
    $result_count = (!empty($search_result)) ? $GLOBALS['pager_total_items'][0] : 0;

    foreach($search_result as $item) {
      $node = node_load($item->sid, NULL, TRUE);
      $author = user_load($node->uid);

      $entry = array(
        'id' => url('node/' . $node->nid, array('absolute' => TRUE)),
        'title' => $node->title,
        'published' => gmdate('c', $node->created),
        'updated' => gmdate('c', $node->changed),
        'summary' => strip_tags($node->teaser),
      );
      $entry['alternate_link'] = $entry['id'];

      if (!empty($author->display_name)) {
        $entry['authors'][$author->uid]['name'] = $author->display_name;
      }
      else {
        $entry['authors'][$author->uid]['name'] = $author->name;
      }
      $entry['authors'][$author->uid]['uri'] = url('user/' . $author->uid, array('absolute' => TRUE));

      $entries .= theme('atom_entry', $entry);
    }
  }

  $feed = array(
    'title' => t('“@title” content search results feed', array('@title' => $query)),
    'alternate_link' => url('ting/search/' . $query, array('absolute' => TRUE)),
    'entries' => $entries,
  );

  // If data was received from cache, set the feeds update time to the
  // cache creation time.
  if ($cache_data) {
    $feed['updated'] = gmdate('c', $cache_data->created);
  }

  if ($result_count > $max_results) {
    $feed['subtitle'] = t('First @max of @count items found.', array('@max' => $max_results, '@count' => $result_count));
  }
  elseif ($result_count > 0) {
    $feed['subtitle'] = format_plural($result_count, 'One item found', '@count items found.', array('@count' => $result_count));
  }
  else {
    $feed['subtitle'] = t('No items found.');
  }

  // Set the correct content type for Atom feeds.
  drupal_set_header('Content-Type: application/atom+xml; charset=utf-8');
  print theme('atom_feed', $feed);
  return;
}

