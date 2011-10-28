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
  $vars['ting_details'] = array( t('Series') => 'ting_series',
                                 t('Conductor') => 'ting_conductor',
                                 t('Orchestra') => 'ting_orchestra',
                                 t('Classiciation') => 'ting_classification_links',
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
                       t('Series') => 'ting_series',
                       t('Conductor') => 'ting_conductor',
                       t('Orchestra') => 'ting_orchestra',
                       t('Classiciation') => 'ting_classification_links',
                       t('Subjects') => 'ting_subjects_links',
                     ),
                   ),
                   array(
                     'title' => NULL,
                   	 'class' => 'additional-details clearfix',
                     'data' => array(
                       t('Original title') => 'ting_source',
                       t('Coauthor') => 'ting_coauthor',
                       t('Translator') => 'ting_translators',
                       t('Photographer') => 'ting_photographers',
                       t('Musician') => 'ting_musicians',
                       t('Reader') => 'ting_readers',
                       t('Illustrator') => 'ting_illustrators',
                       t('Scriptwriter') => 'ting_scriptwriter',
                       t('Actors') => 'ting_actors',
                       t('Editor') => 'ting_editor',
                       t('Type') => 'ting_type',
                       t('Language') => 'ting_language',
                       t('Edition') => 'ting_edition',
                       t('Extent') => 'ting_extent',
                       t('Audience') => 'ting_audience',
                       t('PEGI') => 'ting_pegi_rating',
                       t('Replaces') => 'ting_replaces',
                       t('Replaced by') => 'ting_replaced_by',
                       t('Subtitles') => 'ting_subtitles',
                       t('Publisher') => 'ting_publisher',
                       t('ISBN') => 'ting_isbn',
                       t('ISSN') => 'ting_isssn',
                       t('Record label') => 'ting_record_label',
                       t('URL') => 'ting_uri',
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
  // Definitions of all the record properties we want to extract
  // Each definition consists of:
  // - name: The programatic name of the property. This is used to determined
  //         variable names when extracting the values
  // - entry: The position in the object record where the value is located
  // - namespaces: An array of the allowed namespaces from which to extract
  //               values. If this is omitted then all namespaces are used.
  // - multiple: If set to true the extracted value is an array of all
  //             extracted values. Otherwise only the first value from
  //             a valid namespace is used.
  // - links: If set to true an additional variable is set containing link(s)
  //          to search results for each value.
  $properties = array(
    array('name' => 'title',
    			'entry' => 'dc:title',
          'namespaces' => array(''),),
    // Title appears twice as we prefer the full title
    array('name' => 'title',
    			'entry' => 'dc:title',
          'namespaces' => array('dkdcplus:full'),),
    array('name' => 'volumne_title',
    			'entry' => 'dc:title',
          'namespaces' => array('oss:full'),),
    array('name' => 'publication_date',
          'entry' => 'dc:date',
          'namespaces' => array('')),
    array('name' => 'abstract',
          'entry' => 'dcterms:abstract',
          'namespaces' => array('')),
    array('name' => 'subjects',
          'entry' => 'dc:subject',
          'namespaces' => array('oss:genre', 'dkdcplus:DBCF', 'dkdcplus:DBCS', 'dkdcplus:DBCM', 'dkdcplus:DBCO', 'dkdcplus:DBCN'),
          'multiple' => TRUE,
          'links' => TRUE),
    array('name' => 'classification',
          'entry' => 'dc:subject',
          'namespaces' => array('dkdcplus:DK5', 'dkdcplus:DK5-Text'),
          'multiple' => TRUE,
          'links' => TRUE),
    array('name' => 'series',
          'entry' => 'dc:description',
          'namespaces' => array('dkdcplus:series'),
          'links' => TRUE),
    array('name' => 'conductor',
          'entry' => 'dc:creator',
          'namespaces' => array('oss:cnd'),
          'links' => TRUE),
    array('name' => 'orchestra',
          'entry' => 'dc:contributor',
          'namespaces' => array('oss:mus'),
          'links' => TRUE),
    array('name' => 'source',
          'entry' => 'dc:source',
          'links' => TRUE),
    array('name' => 'type',
          'entry' => 'dc:type'),
    array('name' => 'language',
          'entry' => 'dc:language'),
    array('name' => 'edition',
          'entry' => 'dkdcplus:version',),
    array('name' => 'extent',
          'entry' => 'dcterms:extent'),
    array('name' => 'audience',
          'entry' => 'dcterms:audience',
          'multiple' => TRUE),
    array('name' => 'pegi_rating',
          'entry' => 'dc:description',
          'namespaces' => array('oss:pegi')),
    array('name' => 'replaces',
          'entry' => 'dc:relation',
          'namespaces' => array('replaces'),
    			'links' => TRUE),
    array('name' => 'replaced_by',
          'entry' => 'dc:relation',
          'namespaces' => array('isReplacedBy'),
    			'links' => TRUE),
    array('name' => 'subtitles',
          'entry' => 'dc:language',
          'namespaces' => array('oss:subtitles'),
          'multiple' => TRUE),
    array('name' => 'publisher',
          'entry' => 'dc:publisher',),
    array('name' => 'isbn',
          'entry' => 'dc:identifier',
          'namespaces' => array('dkdcplus:ISBN'),),
    array('name' => 'issn',
          'entry' => 'dc:identifier',
          'namespaces' => array('dkdcplus:ISSN'),),
    array('name' => 'record_label',
          'entry' => 'dc:identifier',),
    array('name' => 'uri',
          'entry' => 'ac:location',),
    array('name' => 'description',
          'entry' => 'dc:description',),
  );

  // Add creators
  // They only vary by name and namespace so they get special handling
  $creator_properties = array('creators' =>  array('dkdcplus:aut', 'dkdcplus:cmp', 'dkdcplus:drt',
                                                   'oss:aut' /* Added for backwards compatability with DKABM 1.1 */),
                              'translators' => array('dkdcplus:trl'),
                              'photographers' => array('dkdcplus:pht'),
                              'musicians' => array('dkdcplus:mus'),
                              'readers' => array('dkdcplus:dkind'),
                              'illustrators' => array('dkdcplus:ill'),
                              'scriptwriter' => array('dkdcplus:aus'),
                              'actors' => array('dkdcplus:act'),
                              'editor' => array('dkdcplus:edt'));

  foreach ($creator_properties as $name => $namespaces) {
    $properties[] = array('name' => $name,
                          'entry' => 'dc:creator',
                          'namespaces' => $namespaces,
                          'multiple' => TRUE,
                          'links' => TRUE,
    );
  }

  $vars = array('ting_local_id' => $object->localId,
                'ting_id' => $object->id,
                'ting_url' => url('ting/object/'.$object->id, array('absolute' => TRUE)));

  // Extract all the properties into variables
  foreach ($properties as $property) {
    // Generate names for the $vars array
    // Names for the foo property will be ting_creators and ting_creators_links
    $var_name = 'ting_' . $property['name'];
    $var_name_links = $var_name . '_' . ((!empty($property['multiple'])) ? 'links' : 'link');

    // Check  if the property is set
    if (!empty($object->record[$property['entry']])) {
      // Values are grouped by namespace. Loop through all of them.
      foreach ($object->record[$property['entry']] as $namespace => $values) {
        // Only look for values in the specified namespaces. If no namespaces have been
        // defined the go through all of them.
        if (!isset($property['namespaces']) || in_array($namespace, $property['namespaces'])) {
          // If the property is multivalue...
          if (!empty($property['multiple'])) {
            // Create arrays to hold values and links if they have not been defined
            if (!isset($vars[$var_name])) {
              $vars[$var_name] = array();
            }
            if (!empty($property['links']) && !isset($vars[$var_name])) {
              $vars[$var_name_links] = array();
            }

            // Add the values
            foreach ($values as $value) {
              $vars[$var_name][] = check_plain($value);
              if (!empty($property['links'])) {
                // Add a link
                $vars[$var_name_links][] = l($value, 'ting/search/' . check_plain($value), array('attributes' => array('class' => _ting_property_classes($property['name'], $namespace, 'ting'))));
              }
            }
          } else {
            // We have a single value so get the first entry
            $value = array_shift($values);
            $vars[$var_name] = check_plain($value);

            if (!empty($property['links'])) {
              // Add a link
              $vars[$var_name_links] = l($value, 'ting/search/' . check_plain($value), array('attributes' => array('class' => _ting_property_classes($property['name'], $namespace, 'ting'))));
            }

            // We have our single value so no need to look further
            break;
          }
        }
      }
    }

    // Remove any duplicate values
    foreach (array($var_name, $var_name_links) as $var) {
      if (is_array($vars[$var])) {
        $vars[$var] = array_unique($vars[$var]);
      }
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