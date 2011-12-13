<?php

/**
 * Theme something from Ting.
 */
function theme_ting_thing($type, $object, $query_args = NULL) {
  static $displayed_relation_types = array(
    'isAuthorDescriptionOf',
    'hasAuthorDescription',
    'hasReview',
    'isReviewOf',
    'hasAnalysis',
    'isAnalysisOf',
    'isSubjectDescriptionOf',
    'hasSubjectDescription',
    'hasFulltext',
  );
  module_invoke_all('ting_add_js', "ting_${type}_view");

  $tabs = array();
  $tabs['main'] = array(
    'title' => t('Materials'),
    'content' => theme("ting_$type", $object, $query_args),
  );

  if (sizeof($object->relations)) {
    $relations = array();
    foreach ($object->relations as $relation) {
      // Remove relations that's not from the local agency.
      if ($relation->ownerId != $object->ownerId) {
        continue;
      }

      if (in_array($relation->relationType, $displayed_relation_types)) {
        $relations[] = $relation;
      }
    }
    if ($relations) {
      $tabs['related'] = array(
        'title' => t('Related'),
        'content' => theme('ting_list', $object->relations),
      );
    }
  }

  drupal_alter('ting_tabs', $tabs, $type, $object);

  return theme('ting_tabs', $tabs, $type == 'collection' ? 'ting-collection' : 'ting-object');
}

/**
 * Template preprocessor for collections of ting objects.
 */
function template_preprocess_ting_collection(&$vars) {
  // Preprocess the collection object here!
 $collection = $vars["collection"];

  if (!empty($collection->objects)) {
    // We use the first object in the collection for determining object information
    // Extract information about this object into variable scope
    $vars = array_merge($vars, ting_collection_preprocess($collection));
  }

  // Build collection details
  // Save preprocessing for ting_preprocess_ting_collection() to allow other
  // modules to alter this array before rendering it.
  $vars['ting_details'] = array( t('Series') => 'ting_series_links',
                                 t('Classiciation') => 'ting_classification_links',
                                 t('Listing') => 'ting_listing_links',
                                 t('Subjects') => 'ting_subjects_links',
  );

  // Allow modules to add action buttons to collection objects
  $vars['buttons'] = array();
  foreach ($collection->objects as $object) {
    $vars['buttons'][$object->id] = module_invoke_all('ting_object_buttons', $object);
  }

  $sorted = array();
  $object_list = array();

  // If we know what query lead to the collection then we can use this for sorting
  if ($query_args = $vars['query_args']) {
    if ($entry = $query_args['entry']) {
      // Build the query for the collection in the search result
      $query = '';

      //Wrap base query
      $query .= (isset($query_args['query'])) ? '('.$query_args['query'].')' : '';

      //Extend query with selected facets
      if (isset($query_args['facets'])) {
        foreach ($query_args['facets'] as $name => $value) {
          $facetArray[] = $name.'="'.$value.'"';
        }
        $query .= ' AND '.implode(' AND ', $facetArray);
      }

      // Get the corresponding entry in the search result
      $result = ting_do_search($query, $entry, 1, array('facets' => array()));

      //Determine the order of object types based on the order of objects
      //in the search result
      if ($search_collection = array_shift($result->collections)) {
        foreach ($search_collection->objects as $object) {
          $sorted[$object->type] = array();
        }
      }

      //Put all objects from original collection into the ordered object type buckets.
      //Types not referenced in the search result placed last
      foreach ($collection->objects as $object) {
        $sorted[$object->type][] = $object;
      }
    }
  }

  //Default to standard sorting
  if (sizeof($sorted) == 0) {
    foreach ($collection->objects as $object) {
      $sorted[$object->type][] = $object;
    }
    // By coincidence, the default type, "Bog", appears alphabetically first.
    ksort($sorted);
  }

  $vars["sorted_collection"] = $sorted;
  $vars['ting_list'] = theme('ting_list', $sorted, TRUE);
}

/**
 * Second preprocessing of Ting collections
 */
function ting_preprocess_ting_collection($vars) {
  // Loop through all levels of ting details to create the item lists
  // We do this here instead of in template_preprocess_ting_object() to let
  // other modules preprocess the ting_details structure before it is rendered
  foreach ($vars['ting_details']  as $title => &$var_name) {
    if (!empty($vars[$var_name])) {
      $var = $vars[$var_name];
      if (!is_array($var)) {
        $var = array($var);
      }
      $var_name = array('class' => _ting_property_classes($var_name),
                        'data' => theme('item_list', $var, $title, 'span'));
    } else {
      unset($vars['ting_details'][$title]);
    }
  }
  $vars['ting_details'] = theme('item_list', $vars['ting_details'], NULL, 'div', array('class' => 'ting-properties'));
}

/**
 * Template preprocessor for ting objects.
 */
function template_preprocess_ting_object(&$vars) {
  $object = $vars['object'];

  // Extract information about the object into variable scope
  $vars = array_merge($vars, ting_object_preprocess($vars['object']));

  // Allow modules to add action buttons to objects
  $vars['buttons'] = module_invoke_all('ting_object_buttons', $object);

  // Determine the cover url
  $image_url = ting_covers_object_url($object, '180_x');
  if ($image_url) {
    $vars['image'] = theme('image', $image_url, '', '', NULL, FALSE);
  }

  // Build a data structure for creating lists of object details.
  // The structure has the following levels:
  // 1. Detail groups: Currently details, additional details and description
  //   2. Object properties: Title, author etc.
  //     3. Property values: These are variable names as we only want to include
  //        them if they are set.
  // Each level is passed though theme('item_list')
  $details = array(array(
                    'title' => NULL,
                    'class' => 'details clearfix',
                    'data' => array(
                       t('Classiciation') => 'ting_classification_links',
                       t('Listing') => 'ting_listing_links',
                       t('Subjects') => 'ting_subjects_links',
                     ),
                   ),
                   array(
                     'title' => NULL,
                      'class' => 'additional-details clearfix',
                     'data' => array(
                       t('Original title') => 'ting_source',
                       t('Contributors') => 'ting_contributors',
                       t('Coauthors') => 'ting_coauthor',
                       t('Writer') => 'ting_writer',
                       t('Translators') => 'ting_translators',
                       t('Photographers') => 'ting_photographers',
                       t('Musicians') => 'ting_musicians',
                       t('Readers') => 'ting_readers',
                       t('Illustrators') => 'ting_illustrators',
                       t('Scriptwriters') => 'ting_scriptwriters',
                       t('Actors') => 'ting_actors',
                       t('Editors') => 'ting_editors',
                       t('Coeditors') => 'ting_coeditors',
                       t('Type') => 'ting_type',
                       t('Language') => 'ting_language',
                       t('Edition') => 'ting_edition',
                       t('Extent') => 'ting_extent',
                       t('Audience') => 'ting_audience_age',
                       t('Media council') => 'ting_audience_media_council',
                       t('PEGI') => 'ting_pegi_rating',
                       t('Related') => 'ting_related',
                       t('Subtitles') => 'ting_subtitles',
                       t('Publisher') => 'ting_publisher',
                       t('ISBN') => 'ting_isbn',
                       t('ISSN') => 'ting_isssn',
                       t('URI') => 'ting_uri',
                       t('ISMN') => 'ting_ismn',
                       t('Record label') => 'ting_record_label',
                     ),
                   ),
                   array(
                     'title' => NULL,
                      'class' => 'descrition clearfix',
                     'data' => array(
                       t('Description') => 'ting_description')
                     ),
                   );

  // If there is more than one author add it to the details
  if (sizeof($vars['ting_creators_links']) > 1) {
    $details[1]['data'] = array_merge(array(t('Creators') => 'ting_creators_links'), $details[1]['data']);
  }

  $vars['ting_details'] = $details;
}

/**
 * Second level preprocessor for ting objects.
 */
function ting_preprocess_ting_object(&$vars) {
  // Loop through all levels of ting details to create the item lists
  // We do this here instead of in template_preprocess_ting_object() to let
  // other modules preprocess the ting_details structure before it is rendered
  foreach ($vars['ting_details'] as $detail_group_title => &$detail_group) {
    foreach ($detail_group['data'] as $title => &$var_name) {
      if (!empty($vars[$var_name])) {
        $var = $vars[$var_name];
        if (!is_array($var)) {
          $var = array($var);
        }
        $var_name = array('class' => _ting_property_classes($var_name),
                          'data' => theme('item_list', $var, $title, 'span'));
      } else {
        unset($detail_group['data'][$title]);
      }
    }
    $detail_group['data'] = theme('item_list', $detail_group['data'], $detail_group['title'], 'div', array('class' => 'ting-properties'));
  }

  $vars['ting_details'] = theme('item_list', $vars['ting_details'],  NULL, 'div');
}

/**
 * Template preprocessor for ting-list.
 */
function template_preprocess_ting_list(&$vars) {
  if (!$vars['sorted']) {
    $sorted = array();
    foreach ($vars['ting_list'] as $object) {
      $object_vars = ting_object_preprocess($object);
      $sorted[$object_vars['ting_type']][] = $object;
    }
    ksort($sorted);
    $vars['ting_list'] = $sorted;
  }
  foreach ($vars['ting_list'] as $type => $object_list) {
    $vars['ting_list'][$type] = "";
    foreach ($object_list as $object) {
      $vars['ting_list'][$type] .= theme('ting_list_item', $object);
    }
  }
}

/**
 * Template preprocessor for ting-list-item.
 */
function template_preprocess_ting_list_item(&$vars) {
  $object = $vars['object'];

  // Extract information about the object into variable scope
  $vars = array_merge($vars, ting_object_preprocess($object));

  $image_url = ting_covers_object_url($object, '80_x');
  if ($image_url) {
    $vars['image'] = theme('image', $image_url, '', '', null, false);
  }

  // Let other modules add buttons.
  $vars['buttons'] = module_invoke_all('ting_object_buttons', $object);
}

/**
 * Template preprocessor for ting tabs.
 */
function template_preprocess_ting_tabs(&$vars) {
  // jQuery UI tabs
  jquery_ui_add('ui.tabs');
  jquery_ui_theme_load();

  drupal_add_js(drupal_get_path('module', 'ting') . '/js/ting.js', 'module');

  $tab_labels = array();
  $tab_content = array();
  foreach ($vars['tabs'] as $id => $tab) {
    $id = form_clean_id($id);
    // Cannot use l/url, they can't generate a link with only a fragment. Also
    // we don't want any 'active' classes.
    $tab_labels[] = array(
      'data' => "<a href='#$id'>" . check_plain($tab['title']) . "</a>",
    );
    $tab_content[$id] = $tab['content'];
  }

  $vars['tabs_labels'] = theme('item_list', $tab_labels);
  $vars['tabs_content'] = $tab_content;
}

/**
 * Theme function to format the custom form element ting_boost_field.
 */
function theme_ting_boost_field($element) {
  return theme('form_element', $element, '<div class="boost-field-instance clear-block">'. $element['#children'] .'</div>');
}


/**
 * Theme function to format the custom form element ting_ranking_field.
 */
function theme_ting_ranking_field($element) {
  return theme('form_element', $element, '<div class="ranking-field-instance clear-block">'. $element['#children'] .'</div>');
}

/**
 * Core function for extracting information from a Ting object into
 * a variable array.

 * @param TingClientObject $object
 */
function ting_object_preprocess(TingClientObject $object) {
  // Key values
  $vars = array('ting_local_id' => $object->localId,
                'ting_id' => $object->id,
                'ting_url' => url('ting/object/' . $object->id, array('absolute' => TRUE)));

  // Definitions of all the record properties we want to extract
  // Keys are the programatic names of properties. This is used to determined
  // variable names when extracting the values.
  // Each definition consists of:
  // - entry: The position in the object record where the value is located
  // - namespaces: An array of the allowed namespaces from which to extract
  //               values. If this is omitted then all namespaces are used.
  // - !namespaces: An array of namespaces to ignore. If this is omitted then
  //                all namespaces are allowed.
  // - multiple: If set to true the extracted value is an array of all
  //             extracted values. Otherwise only the first value from
  //             a valid namespace is used.
  // - links: If set to true an additional variable is set containing link(s)
  //          to search results for each value.
  // - link_facet: If set this facet will be used when creating links instead of
  //               broad searches
  // - html: Whether to allow escape any HTML content contained within the field
  // Each property may contain multiple definitions. Handling depends on the
  // multiple value.
  $properties = array(
    'title' => array(
      'entry' => 'dc:title',
      'namespaces' => array(''),
    ),
    'title_full' => array(
      'entry' => 'dc:title',
      'namespaces' => array('dkdcplus:full'),
    ),
    'volumne_title' => array(
      'entry' => 'dc:title',
      'namespaces' => array('oss:full'),
    ),
    'creators' => array(
      'entry' => 'dc:creator',
      '!namespaces' => array('oss:sort'),
      'multiple' => TRUE,
      'links' => TRUE,
      'link_facet' => 'facet.creator',
    ),
    'publication_date' => array(
      'entry' => 'dc:date',
      'namespaces' => array(''),
    ),
    'abstract' => array(
      'entry' => 'dcterms:abstract',
      'namespaces' => array(''),
      'html' => TRUE,
    ),
    'description' => array(
      'entry' => 'dc:description',
      'namespaces' => array(''),
    ),
    'subjects' => array(
      'entry' => 'dc:subject',
      'namespaces' => array('dkdcplus:genre', 'dkdcplus:DBCF', 'dkdcplus:DBCS', 'dkdcplus:DBCM', 'dkdcplus:DBCO', 'dkdcplus:DBCN', 'dcterms:spatial', 'dcterms:temporal',
                            'oss:genre' /* Included for backwards compatability with DKABM 1.x' */),
      'multiple' => TRUE,
      'links' => TRUE,
    ),
    'classification' => array(
      'entry' => 'dc:subject',
      'namespaces' => array('dkdcplus:DK5'),
      'multiple' => TRUE,
      'links' => TRUE,
    ),
    'listing' => array(
      'entry' => 'dc:subject',
      'namespaces' => array('dkdcplus:DK5-Text'),
      'multiple' => TRUE,
      'links' => TRUE,
    ),
    'series' => array(
      array(
        'entry' => 'dc:title',
        'namespaces' => array('dkdcplus:series'),
        'multiple' => TRUE,
      ),
      array(
        'entry' => 'dc:description',
        'namespaces' => array('dkdcplus:series'),
        'multiple' => TRUE,
      ),
    ),
    'source' => array(
      'entry' => 'dc:source',
      'links' => TRUE,
    ),
    'type' => array(
      'entry' => 'dc:type'
    ),
    'language' => array(
      'entry' => 'dc:language',
      'namespaces' => array(''),
    ),
    'language_iso639_2' => array(
      'entry' => 'dc:language',
      'namespaces' => array('dcterms:ISO639-2'),
    ),
    'edition' => array(
      'entry' => 'dkdcplus:version',
    ),
    'extent' => array(
      'entry' => 'dcterms:extent'
    ),
    'record_label' => array(
      'entry' => 'dc:identifier',
      'namespaces' => array(''),
    ),
    'audience_age' => array(
      'entry' => 'dcterms:audience',
      'namespaces' => array('dkdcplus:age'),
      'multiple' => TRUE
    ),
    'audience_media_council' => array(
      'entry' => 'dcterms:audience',
      'namespaces' => array('dkdcplus:medieraad'),
      'multiple' => TRUE
    ),
    'pegi_rating' => array(
      'entry' => 'dcterms:audience',
      'namespaces' => array('dkdcplus:pegi')
    ),
    'related' => array(
      'entry' => 'dc:relation',
      'namespaces' => array('dcterms:isPartOf', 'dcterms:hasPart', 'dcterms:isReplacedBy', 'dcterms:replaces', 'dcterms:references'),
      'links' => TRUE
    ),
    'subtitles' => array(
      'entry' => 'dc:language',
      'namespaces' => array('dkdcplus:subtitles',
                            'oss:subtitles'  /* Included for backwards compatability with DKABM 1.x' */),
      'multiple' => TRUE
    ),
    'publisher' => array(
      'entry' => 'dc:publisher',
    ),
    'isbn' => array(
      'entry' => 'dc:identifier',
      'namespaces' => array('dkdcplus:ISBN'),
    ),
    'issn' => array(
      'entry' => 'dc:identifier',
      'namespaces' => array('dkdcplus:ISSN'),
    ),
    'uri' => array(
      'entry' => 'dc:identifier',
      'namespaces' => array('dkdcplus:URI')
    ),
    'ismn' => array(
      'entry' => 'dc:identifier',
      'namespaces' => array('dkdcplus:ISMN')
    ),
  );

  // Add contributors
  // They only vary by name and namespace so they get special handling
  $contrib_properties = array('actors' => array('dkdcplus:act'),
                              'coauthor' => array('dkdcplus:dkmed'),
                              'coeditors' => array('dkdcplus:dkmdt'),
                              'conductors'=> array('oss:cnd'),
                              'draftsmen' => array('dkdcplus:drm'),
                              'editors' => array('dkdcplus:edt'),
                              'illustrators' => array('dkdcplus:ill'),
                              'interviewee' => array('dkdcplus:ive'),
                              'musicians' => array('dkdcplus:mus'),
                              'orchestras' => array('oss:mus'),
                              'photographers' => array('dkdcplus:pht'),
                              'readers' => array('dkdcplus:dkind'),
                              'scriptwriters' => array('dkdcplus:aus'),
                              'translators' => array('dkdcplus:trl'),
                              'writers' => array('dkdcplus:dktek'),
                              'contributors' => NULL, // All contributors. Any contributor with a recognized function will be removed later on
  );

  foreach ($contrib_properties as $name => $namespaces) {
    $properties[$name] = array( 'entry' => 'dc:contributor',
                                'namespaces' => $namespaces,
                                'multiple' => TRUE,
                                'links' => TRUE);
  }

  // Extract all the properties into variables

  foreach ($properties as $name => $definitions) {
    // Generate names for the $vars array
    // Names for the foo property will be ting_creators and ting_creators_links
    $var_name = 'ting_' . $name;
    $var_name_links = $var_name . '_' . ((!empty($definitions['multiple'])) ? 'links' : 'link');

    // Convert single definitions to an array of definitions
    if (!isset($definitions[0])) {
      $definitions = array($definitions);
    }

    // Loop through all definitions
    foreach ($definitions as $definition) {
      if (!empty($object->record[$definition['entry']])) {
        // Values are grouped by namespace. Loop through all of them.
        foreach ($object->record[$definition['entry']] as $namespace => $values) {
          // Only look for values in the specified namespaces. If no namespaces have been
          // defined the go through all of them.
          if ((!isset($definition['namespaces']) || in_array($namespace, $definition['namespaces'])) &&
              (!isset($definition['!namespaces']) || !in_array($namespace, $definition['!namespaces']))) {
            // If the property is multivalue...
            if (!empty($definition['multiple'])) {
              // Create arrays to hold values and links if they have not been defined
              if (!isset($vars[$var_name])) {
                $vars[$var_name] = array();
              }
              if (!empty($definition['links']) && !isset($vars[$var_name])) {
                $vars[$var_name_links] = array();
              }

              // Add the values
              foreach ($values as $value) {
                $vars[$var_name][] = check_plain($value);
                if (!empty($definition['links'])) {
                  $facet = (!empty($definition['link_facet'])) ? $definition['link_facet'] : NULL;
                  // Add a link
                  $vars[$var_name_links][] = _ting_property_value_link($value, $name, $namespace, $facet);
                }
              }
            } else {
              // We have a single value so get the first entry
              $value = array_shift($values);
              $vars[$var_name] = check_plain($value);

              if (!empty($definition['links'])) {
                $facet = (!empty($definition['link_facet'])) ? $definition['link_facet'] : NULL;
                // Add a link
                $vars[$var_name_links] = _ting_property_value_link($value, $name, $namespace, $facet);
              }

              // We have our single value so no need to look further
              break 2;
            }
          }
        }
      }
    }

    // Remove any duplicate values
    foreach (array($var_name, $var_name_links) as $var) {
      if (isset($vars[$var]) && is_array($vars[$var])) {
        $vars[$var] = array_unique($vars[$var]);
      }
    }
  }

  // Property post processing

  // Remove full title if same as normal title
  if ($vars['ting_title'] == $vars['ting_title_full']) {
    unset($vars['ting_title_full']);
  }

  // Filter contributors listed in other functions
  $contrib_var_names = array('ting_contributors', 'ting_contributors_links');
  // Loop through all contributor type properties...
  foreach ($contrib_properties as $name => $namespace) {
    // ... and all permutations of the individual property
    $var_names = array('ting_' . $name, 'ting_' . $name . '_links');
    foreach ($var_names as $var_name) {
      // If the property is set and not a contributor variable name in it self...
      if (isset($vars[$var_name]) && !in_array($var_name, $contrib_var_names)) {
        // ... loop through all values
        foreach ($vars[$var_name] as $value) {
          // If the value is also contained within the general contributor values...
          foreach ($contrib_var_names as $contrib_var_name) {
            $key = array_search($value, $vars[$contrib_var_name]);
            if ($key !== FALSE) {
              // ... then remove it
              unset($vars[$contrib_var_name][$key]);
            }
          }
        }
      }
    }
  }

  // Process series information
  // This could be handled more elegantly if we had better structured data.
  // For now we have to work with what we got to convert titles to links
  // Series information appear in the following formats:
  // - Samhørende: [title 1] ; [title 2] ; [title 3]
  // - [volumne number]. del af: [title]
  // - [title] : [volumne number]
  if (isset($vars['ting_series'])) {
    $vars['ting_series_links'] = array();
    foreach ($vars['ting_series'] as $series) {
      // Any text appearing before series titles
      $prefix = '';
      // Series titles.
      // This will already have been passed through check_plain so we decode
      // it to avoid double encoding.
      $titles = htmlspecialchars_decode($series);
      // Check for prefixes in the series information
      $prefixes = array('Samhørende:', 'del af:');
      foreach ($prefixes as $p) {
        // If prefix is found then split titles and prefix
        if (($pos = stripos($series, $p)) !== FALSE) {
          $split = drupal_strlen($p) + $pos;
          $prefix = drupal_substr($series, 0, $split);
          $titles = drupal_substr($series, $split + 1);

          break;
        }
      }

      // Multiple titles are separated by ' ; '. Explode to iterate over them
      $titles = explode(' ; ', $titles);
      foreach ($titles as &$title) {
        $title = trim($title);
        // Some title elements are actually volumne numbers. Do not convert these to links
        if (!is_numeric($title)) {
          $title = _ting_property_value_link($title, 'dc:title', 'dkdcplus:series');
        }
      }
      // Reassemple titles
      $titles = implode(' ; ', $titles);

      // Create a link string
      $vars['ting_series_links'][] = (!empty($prefix) ? check_plain($prefix) . ' ' : '') . $titles;
    }
  }

  return $vars;
}

/**
 * Core function for extracting information from a Ting collection into
 * a variable array.

 * @param TingClientObjectCollection $collection
 */
function ting_collection_preprocess(TingClientObjectCollection $collection) {
  // The first object in the collection will be the one to determine
  // most of the variable values
  $vars = ting_object_preprocess($collection->objects[0]);

  // Override url if collection contains multiple objects
  if (sizeof($collection->objects) > 1) {
    $vars['ting_url'] = url('ting/collection/'. $collection->objects[0]->id, array('absolute' => TRUE));
  }

  // Define aggregate values
  $aggregate_properties = array('subjects', 'type');

  // Extract values for all objects
  $object_vars = array();
  foreach ($collection->objects as $object) {
    $object_vars[] = ting_object_preprocess($object);
  }

  // Loop through all aggregate properties
  foreach ($aggregate_properties as $property) {
    // Generate names for the $vars array
    // Names for the foo property will be ting_creators and ting_creators_links
    $var_name = 'ting_' . $property;
    $var_name_link = $var_name . '_link';
    $var_name_links = $var_name . '_links';

    // Loop through all possible variable names
    foreach(array($var_name, $var_name_link, $var_name_links) as $var_name) {
      $aggregate = array();

      // Loop through all object variables
      foreach ($object_vars as $object_var) {
        // If the object variables contain the variable name then we have
        // something to add to our aggregates
        if (isset($object_var[$var_name])) {
          // Values may be single values or arrays. Convert the singles
          // to arrays to make them easier to work with
          if (!is_array($object_var[$var_name])) {
            $object_var[$var_name] = array($object_var[$var_name]);
          }

          foreach($object_var[$var_name] as $var_value) {
            // New variable. Add value as key and set count to 0
            if (empty($aggregate[$var_value])) {
              $aggregate[$var_value] = 0;
            }

            // Add count to value
            $aggregate[$var_value]++;
          }
        }
      }

      if (!empty($aggregate)) {
        // Sort by count and return keys
        arsort($aggregate);
        $vars[$var_name] = array_keys($aggregate);
      }
    }
  }

  return $vars;
}

/**
 * Utility function to generate a link for a Ting object property.
 *
 * @param string $value The property valye
 * @param string $name The name of the property
 * @param string $namespace The namespace of the property
 * @param string $facet A facet to use when creating the link
 */
function _ting_property_value_link($value, $name, $namespace = NULL, $facet = NULL) {
  // TODO: This creates a hard dependency to ting_search.
  // Could this be decoupled using a hook or the like?
  $path = 'ting/search/' . (($facet) ? $facet . '=' : '' ) . '"' . $value . '"';
  $options = array(
              'attributes' => array(
                'class' => _ting_property_classes($name, $namespace, 'ting'))
             );
  return l($value, $path, $options);
}

/**
 * Utility function to generate a string of  CSS classes for a Ting object property.
 *
 * @param string $name The name of the property
 * @param string $namespace The namespace of the property
 * @param string $prefix A prefix to preprend to all classes
 */
function _ting_property_classes($name, $namespace = NULL, $prefix = NULL) {
  $classes = array();

  // Properties may have link(s) appended - remove this
  if ($pos = strrpos($name, '_link', -6)) {
    $name = substr($name, 0, $pos);
  }

  // First generate a class for the property with optional prefix
  // in the format prefix-property
  // All characters are set in lowercase and everything execept letters
  // and numbers are replaced with -.
  $classes[] = (!empty($prefix) ? $prefix . '-' : '' ) . preg_replace('/[^a-z1-9-]/', '-', strtolower($name));
  if (!empty($namespace)) {
    // Next add a class for the property with prefix prepended and
    // namespace appended.
    // Use self to reuse character replacement.
    $classes[] = _ting_property_classes($classes[0] . '-' . $namespace);
  }

  return implode(' ', $classes);
}
