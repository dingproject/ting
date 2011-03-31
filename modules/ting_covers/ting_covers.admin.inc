<?php

/**
 * Admin settings form for Ting covers.
 */
function ting_covers_admin_settings_form(&$form_state) {
  $form = array();

  $form['#attributes'] = array('enctype' => 'multipart/form-data');

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cover settings'),
  );

  $form['settings']['cache_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Cover cache lifetime'),
    '#options' => array(
      '86400' => t("24 hours"),
      '1209600' => t("2 weeks"),
    ),
    '#default_value' => variable_get('ting_covers_cache_lifetime', 86400),
  );

  $form['settings']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
    '#submit' => array('ting_covers_admin_settings_save'),
  );

  $form['default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default cover image'),
    '#tree' => FALSE,
  );

  if ($file = ting_covers_default_image()) {
    $form['default']['current'] = array(
      '#value' => implode(array(
        t('Your current default image:'),
        ' ',
        theme_image($file->filepath),
      )),
    );
  }
  else {
    $form['default']['current'] = array(
      '#value' => t('You currently have no default image. You can upload one below.'),
    );
  }

  $form['default']['default_image_upload'] = array(
    '#type' => 'file',
    '#title' => t('Upload new'),
    '#description' => t('Upload a new default image. This should be of a reasonable size, and preferably in PNG or JPEG format.'),
  );

  $form['default']['upload'] = array(
    '#type' => 'submit',
    '#value' => t('Upload'),
    '#validate' => array('_ting_covers_admin_validate_default_image_upload'),
  );

  return $form;
}

/**
 * Submission handler.
 */
function ting_covers_admin_settings_save($form, &$form_state) {
  variable_set('ting_covers_cache_lifetime', $form_state['values']['cache_lifetime']);
}

/**
 * Validation for settings form.
 */
function _ting_covers_admin_validate_default_image_upload($form, &$form_state) {
  // Validate the file upload.
  $file = file_save_upload('default_image_upload', array(
    'file_validate_is_image' => array(),
    'file_validate_extensions' => array('png gif jpg jpeg'),
  ));

  if ($file) {
    // Delete the previous image file.
    $fid = variable_get('ting_covers_default_image_fid', FALSE);
    if ($fid) {
      // Load file object from database.
      $old_file = db_fetch_object(db_query('SELECT * FROM {files} WHERE fid = %d', $fid));

      // Remove the file.
      if (file_exists($old_file->filepath)) {
        file_delete($old_file->filepath);
      }

      // And remove it from the files table.
      db_query('DELETE FROM {files} WHERE fid = %d', $fid);
    }

    // Get the image info to get the correct extension for the uploaded file.
    $info = image_get_info($file->filepath);

    if (file_move($file, 'ting/covers/default_image.' . $info['extension'], FILE_EXISTS_REPLACE)) {
      // Mark the file for permanent storage.
      file_set_status($file, FILE_STATUS_PERMANENT);

      // Update the files table.
      drupal_write_record('files', $file, 'fid');

      // Store the file fid
      variable_set('ting_covers_default_image_fid', $file->fid);
    }
    else {
      form_set_error('default][default_image_upload', t('Failed to write the uploaded file to ting/covers in the site’s files folder.'));
    }
  }
  else {
    form_set_error('default][default_image_upload', t('Error validating file upload. Please upload an image in PNG or JPEG format.'));
  }
}

