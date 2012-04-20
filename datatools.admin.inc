<?php

/**
 * @file
 * Administrative page callbacks for the datatools module.
 */

/**
 * Menu callback
 */
function datatools_list($form_state) {

  $form = array();
  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Datatools'),
    '#description' => t('Data comparison and manipulation tools.')
  );

  $form['fs']['description'] = array(
    '#title' => t('Data utility tools'),
    '#value' => t('things.')
  );

  return $form;
}

/**
 * Menu callback
 */
function datatools_search_form($form_state) {

  $form = array();
  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('datatools search'),
    '#description' => t('Enter || separated list of SKUs; receive bacon ≈')
  );

  if ($form_state['values']['mu']) {
    $form['fs']['mu'] = array(
      '#type' => 'textarea',
      '#title' => t('Nodes!'),
      '#description' => t('If you can read this message, you have results!'),
      '#cols' => 150,
      '#rows' => 20,
      '#default_value' => $form_state['values']['mu'],
    );
  }

  $form['fs']['datatools'] = array(
    '#type' => 'textarea',
    '#title' => t('datatools'),
    '#description' => t('Enter a pipe-separated list of SKUs. Each set can be on a new line.'),
    '#default_value' => $form_state['values']['datatools'],
    '#cols' => 150,
    '#rows' => 20,
  );

  $form['fs']['submit'] = array(
    '#type' => 'submit',
    '#default_value' => t('Search'),
  );

  $form['#redirect'] = FALSE;

  return $form;
}

function datatools_search_form_validate($form, &$form_state) {
  if ($form_state['values']['datatools'] == '') {
    drupal_set_message(t('You should really enter something for this to be of any use'), 'warning');
  }
}

function datatools_search_form_submit($form, &$form_state) {

  $rows = explode("\n", trim($form_state['values']['datatools']));
  $data = '';
  $i = 0;
  $missing = array();
  foreach($rows as $row) {
    $skus = explode('||', $row);
    $nidata = '';
    $nids = array();
    foreach ($skus as $sku) {
      $sku = trim($sku);
      $nid = db_result(db_query("SELECT nid FROM {uc_products} WHERE (model = '%s')", $sku));
      $type = db_result(db_query("SELECT type FROM {node} WHERE (nid = '%s')", $nid));
      drupal_set_message('Node '. $nid .' is type '. $type);
      if (trim($nid)) {
        $nids[$nid] = $nid;
        //$nidata .= trim($nid) .'||';
      }
      else {
        $missing[$sku] = $sku;
      }
    }

    $data .= implode('||', $nids) . "\n";
    $i++;
  }

  if (!empty($missing)) {
    $missing = implode(', ', $missing);
    drupal_set_message('<h2>The following SKUs don\'t have NIDs</h2>'. $missing, 'warning');
  }

  $form_state['storage']['mu'] = 'foobar112';
  $form_state['values']['mu'] = $data;

//  drupal_set_message('<pre>'. $data .'</pre>');
}


/**
 * menu callback
 */
function datatools_search_taxonomy_form($form_state) {

  $form = array();
  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy search'),
    '#description' => t('Enter newline separated list of term hierarchies')
  );

  $form['fs']['fs2'] = array(
    '#type' => 'fieldset',
    '#title' => t('Parse settings'),
    '#description' => t('Utility fields to manipulate the text or parse multiple terms'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fs']['fs2']['str_replace'] = array(
    '#type' => 'textfield',
    '#title' => t('String replace'),
    '#description' => t('Replace a string on the entire taxonomy term row.'),
    '#default_value' => $form_state['values']['str_replace'] ? $form_state['values']['str_replace'] : '_',
  );
  $form['fs']['fs2']['term_parser'] = array(
    '#type' => 'textfield',
    '#title' => t('Term parser'),
    '#description' => t('Enter the parser between terms.'),
    '#default_value' => $form_state['values']['term_parser'] ? $form_state['values']['term_parser'] : ':',
  );
  $form['fs']['fs2']['path_parser'] = array(
    '#type' => 'textfield',
    '#title' => t('Path parser'),
    '#description' => t('Enter the parser to be used between paths.'),
    '#default_value' => $form_state['values']['path_parser'] ? $form_state['values']['path_parser'] : ',',
  );

  if ($form_state['storage']['mu']) {
    $form['fs']['mu'] = array(
      '#type' => 'markup',
      '#title' => t('Term matches!'),
      '#description' => t('If you can read this message, you have results!'),
      '#default_value' => $form_state['storage']['mu'],
      '#value' => $form_state['storage']['mu'],
    );
  }

  $vocabs = taxonomy_get_vocabularies();
  foreach ($vocabs as $vid => $vocab) {
    $options[$vid] = $vocab->name;
  }

  $form['fs']['vocabid'] = array(
    '#type' => 'select',
    '#title' => t('Select vocabulary'),
    '#default_value' => $form_state['values']['vocabid'],
    '#options' => $options,
  );
  $form['fs']['term_separator'] = array(
    '#type' => 'textfield',
    '#title' => t('Term separator'),
    '#description' => t('Enter the separator to be used between terms in a path.'),
    '#default_value' => $form_state['values']['term_separator'] ? $form_state['values']['term_separator'] : '>>',
  );
  $form['fs']['path_separator'] = array(
    '#type' => 'textfield',
    '#title' => t('Path separator'),
    '#description' => t('Enter the separator to be used between paths.'),
    '#default_value' => $form_state['values']['path_separator'] ? $form_state['values']['path_separator'] : ',',
  );

  $form['fs']['settings'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select options'),
    '#options' => array(
      1 => t('Compact view (no duplicates)'),
      2 => t('Split results (list of passes then fails)'),
      3 => t('Preserve rows (keep terms on same line - does not work with and overrides above options)'),
      4 => t('Hide failed terms'),
      5 => t('Hide parent terms'),
    ),
    '#default_value' => $form_state['values']['settings'] ? $form_state['values']['settings'] : array(),
  );

  $form['fs']['terms'] = array(
    '#type' => 'textarea',
    '#title' => t('Terms'),
    '#description' => t('Enter a newline-separated list of terms.'),
    '#default_value' => $form_state['values']['terms'],
    '#cols' => 150,
    '#rows' => 20,
  );

  $form['fs']['submit'] = array(
    '#type' => 'submit',
    '#default_value' => t('Search'),
  );

  $form['#redirect'] = FALSE;

  return $form;
}

function datatools_search_taxonomy_form_submit($form, &$form_state) {
  // Get formatting settings.
  $vocabid = trim($form_state['values']['vocabid']);
  $path_separator = trim($form_state['values']['path_separator']);
  $term_separator = trim($form_state['values']['term_separator']);
  $terms = explode("\n", trim($form_state['values']['terms']));

  $str_replace = trim($form_state['values']['str_replace']);
  $term_parser = trim($form_state['values']['term_parser']);
  $path_parser = trim($form_state['values']['path_parser']);

  $settings = $form_state['values']['settings'];
  $compact_view = $settings[1] == 1;
  $split_results = $settings[2] == 2;
  $preserve_rows = $settings[3] == 3;
  $hide_failed = $settings[4] == 4;
  $hide_parents = $settings[5] == 5;

//  $tree = taxonomy_get_tree($vocabid);

  // Build the nested taxonomy tree.
  $tree = taxonomy_get_nested_tree(intval($vocabid), 4);

  $rows = array();

  // For each row of term sets.
  foreach($terms as $term_set) {
    // Format the term string a bit and create term array on separator.
    if ($str_replace) {
      $term_set = str_replace($str_replace, ' ', $term_set);
    }
    if ($path_parser) {
      $term_array = explode($path_parser, $term_set);
    }
    else {
      $term_array = array($term_set);
    }
    // This array is to hold the preserved rows array.
    $row = array();

    // For each set of terms in the row.
    foreach ($term_array as $ta) {
      // Explode on the hierarchy.
      if ($term_parser) {
        $search_terms = explode($term_parser, $ta);
      }
      else {
        $search_terms = $ta;
      }
      // Reset the starting level as it's passed by reference.
      $level = 0;
      // Check if the term hierarchy exists.
      $f = taxonomy_path_exists($level, $search_terms, $tree);
      // Set the message depending on if path exists.
      $has_path = $f ? '<span style="color:green;">HAS PATH!</span>' : '<span style="color:red;">NO PATH :(</span>';
      // Create the hierarchy string for output.
      if ($hide_parents) {
        $term_hash = $search_terms[sizeof($search_terms) - 1];
      }
      else {
        if ($term_separator && is_array($search_terms)) {
          $term_hash = implode(htmlentities($term_separator), $search_terms);
        }
      }
      if ($preserve_rows) {
        if (!($hide_failed && !$f)) {
          $row[] = '<span style="color:' . ($f ? 'green' : 'red') . ';">' . $term_hash . '</span>';
        }
      }
      else if ($split_results) {
        if ($compact_view) {
          $rows[$f ? 'passing' : 'failing'][$term_hash] = array($term_hash, $has_path);
        }
        else {
          $rows[$f ? 'passing' : 'failing'][] = array($term_hash, $has_path);
        }
      }
      else {
        if ($compact_view) {
          $rows[$term_hash] = array($term_hash, $has_path);
        }
        else {
          $rows[] = array($term_hash, $has_path);
        }
      }
    }
    if ($preserve_rows) {
      $rows[][] = implode($path_separator, $row);
    }
  }
  if ($preserve_rows) {
    $data = theme('table', array('Terms'), $rows);
  }
  else if ($split_results) {
    $data = theme('table', array('terms', 'result'), $rows['passing'], array(), t('Passing'));
    $data .= theme('table', array('terms', 'result'), $rows['failing'], array(), t('Failing'));
  }
  else {
    $data = theme('table', array('terms', 'result'), $rows);
  }
  $form_state['storage']['mu'] = $data;
  $form_state['storage']['settings'] = $settings;
}

function taxonomy_path_exists(&$level, $path, $tree, $debug = FALSE) {
  if (empty($path) || empty($tree)) {
    return FALSE;
  }
  if ($debug) dpm('Running at level ' . $level . ' on path point ' . $path[$level]);
  foreach ($tree as $branch) {
    // If we have a path, follow it.
    if (trim($branch->name) == trim($path[$level])) {
      // If we've reached the last leaf, return TRUE.
      if ($level == sizeof($path) - 1) {
        return TRUE;
      }
      // Otherwise, level up and recursively search.
      else {
        if ($debug) dpm('Leveling up on ' . $level);
        $level++;
        return taxonomy_path_exists($level, $path, $branch->children);
      }
    }
    else if ($level == sizeof($path)) {
      if ($debug) dpm($level . ' is sizeof path: ' . sizeof($path));
      return FALSE;
    }
  }
}

function taxonomy_get_nested_tree($terms = array(), $max_depth = NULL, $parent = 0, $parents_index = array(), $depth = 0) {
  if (is_int($terms)) {
    $terms = taxonomy_get_tree($terms);
  }

  foreach($terms as $term) {
    foreach($term->parents as $term_parent) {
      if ($term_parent == $parent) {
        $return[$term->tid] = $term;
      }
      else {
        $parents_index[$term_parent][$term->tid] = $term;
      }
    }
  }

  foreach($return as &$term) {
    if (isset($parents_index[$term->tid]) && (is_null($max_depth) || $depth < $max_depth)) {
      $term->children = taxonomy_get_nested_tree($parents_index[$term->tid], $max_depth, $term->tid, $parents_index, $depth + 1);
    }
  }

  return $return;
}

/**
 * menu callback
 */
function datatools_search_filefind_form($form_state) {

  $form = array();
  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('File search'),
    '#description' => t('Enter newline separated list of files'),
  );

  $form['fs']['settings'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select options'),
    '#options' => array(
      1 => t('Split results (list of passes then fails)'),
      2 => t('Hide failed files'),
      3 => t('Hide passed files'),
      4 => t('Skip blank files'),
      5 => t('Hide duplicates'),
    ),
    '#default_value' => $form_state['values']['settings'] ? $form_state['values']['settings'] : array(),
  );

  $form['fs']['dirname'] = array(
    '#type' => 'textfield',
    '#title' => t('Directory name'),
    '#description' => t('Enter the directory name from the document root. Example: sites/default/files/images'),
    '#default_value' => $form_state['values']['dirname'] ? $form_state['values']['dirname'] : 'sites/default/files',
  );

  if ($form_state['storage']['file_matches']) {
    $form['fs']['mu'] = array(
      '#type' => 'markup',
      '#title' => t('File matches!'),
      '#description' => t('If you can read this message, you have results!'),
      '#value' => $form_state['storage']['file_matches'],
    );
  }

  $form['fs']['filenames'] = array(
    '#type' => 'textarea',
    '#title' => t('File names'),
    '#description' => t('Enter newline separated list of files'),
    '#default_value' => $form_state['values']['filenames'],
    '#rows' => 20,
    '#cols' => 100,
  );

  $form['fs']['submit'] = array(
    '#type' => 'submit',
    '#default_value' => t('Search'),
  );

  $form['#redirect'] = FALSE;

  return $form;
}

function datatools_search_filefind_form_submit($form, &$form_state) {

  $settings = $form_state['values']['settings'];
  $split_results = $settings[1] == 1;
  $hide_failed = $settings[2] == 2;
  $hide_passed = $settings[3] == 3;
  $skip_blank = $settings[4] == 4;
  $hide_dups = $settings[5] == 5;


  $filenames = explode("\n", trim($form_state['values']['filenames']));
  $dirname = $_SERVER['DOCUMENT_ROOT'] . '/' . trim($form_state['values']['dirname'], '/');
  $dirs_exclude = array('.', '..');

  if (file_exists($dirname)) {
/*
    // Get all the files in the directory.
    if ($handle = opendir($dirname)) {
      while (false !== ($entry = readdir($handle))) {
        if (!in_array($entry, $dirs_exclude)) {
          $entries[] = $entry;
        }
      }
      closedir($handle);
    }
    // Sort them .. for display.
    sort($entries);
*/
    $exists_tracker = array(
      'exists' => 0,
      'notexists' => 0,
    );

    $dup_check = array();

    // Check each filename against the list of directories.
    foreach ($filenames as $filename) {
      $filename = trim($filename);

      if (in_array($filename, $dup_check) && $hide_dups) {
        continue;
      }

      $dup_check[] = $filename;

      if (!$filename && $skip_blank) {
        continue;
      }
      else if (!$filename) {
        $rows[] = array('', 'blank');
        continue;
      }
//      $exists = in_array($filename, $entries);
      $exists = file_exists($dirname . '/' . $filename);
      $exists_tracker[$exists ? 'exists' : 'notexists']++;

//      dpm($filename . ' ' . (file_exists($dirname . '/' . $filename) ? '<span style="color:green;">EXISTS</span>' : '<span style="color:red;">DOES NOT EXIST</span>'));
      if ($hide_failed && $exists) {
        $rows[] = array($filename, $exists ? '<span style="color:green;">EXISTS</span>' : '<span style="color:red;">DOES NOT EXIST</span>');
      }
      else if ($hide_passed && !$exists) {
        $rows[] = array($filename, $exists ? '<span style="color:green;">EXISTS</span>' : '<span style="color:red;">DOES NOT EXIST</span>');
      }
      else if (!$hide_passed && !$hide_failed) {
        $rows[] = array($filename, $exists ? '<span style="color:green;">EXISTS</span>' : '<span style="color:red;">DOES NOT EXIST</span>');
      }
    }
  }

  drupal_set_message(sprintf('%d files exist and %d do not exist', $exists_tracker['exists'], $exists_tracker['notexists']));

  $data = theme('table', array('filename', 'Exists'), $rows);
  $form_state['storage']['filenames'] = $form_state['values']['filenames'];
  $form_state['storage']['settings'] = $form_state['values']['settings'];
  $form_state['storage']['file_matches'] = $data;
}

function datatools_search_users_form($form_state) {
  $form = array();

  if ($form_state['values']['data']) {
    $form['fs-results'] = array(
      '#type' => 'fieldset',
      '#title' => t('Results'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['fs-results']['results'] = array(
      '#title' => t('Results'),
      '#value' => $form_state['values']['data'],
    );

    if ($form_state['values']['uids']) {
      $form['fs-results']['mu'] = array(
        '#type' => 'textarea',
        '#title' => t('UIDs'),
        '#description' => t('If you can read this message, you have results!'),
        '#cols' => 150,
        '#rows' => 20,
        '#default_value' => $form_state['values']['uids'],
      );
    }
  }

  $form['fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email addresses'),
  );

  $form['fs']['settings'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select options'),
    '#options' => array(
      1 => t('Only display status (found, missing)'),
      2 => t('Split results (list of passes then fails)'),
      3 => t('DPM'),
      4 => t('Pass only'),
      5 => t('Fail only'),
    ),
    '#default_value' => $form_state['values']['settings'] ? $form_state['values']['settings'] : array(),
  );

  $form['fs']['emails'] = array(
    '#type' => 'textarea',
    '#title' => t('Email addresses'),
    '#description' => t('Enter an email address on each line to look up.'),
    '#default_value' => $form_state['values']['emails'],
    '#cols' => 150,
    '#rows' => 20,
  );

  $form['fs']['submit'] = array(
    '#type' => 'submit',
    '#default_value' => t('Search'),
  );

  $form['#redirect'] = FALSE;

  return $form;
}

function datatools_search_users_form_submit($form, &$form_state) {
  $output = $uids = array();

  $settings = $form_state['values']['settings'];
  $status_only = $settings[1] == 1;
  $split_results = $settings[2] == 2;
  $dpm = $settings[3] == 3;
  $pass_only = $settings[4] == 4;
  $fail_only = $settings[5] == 5;

  $emails = explode("\n", trim($form_state['values']['emails']));
  $results = array(
    'missing' => 0,
    'found' => 0,
  );

  foreach ($emails as $email) {
    $email = trim($email);
    $uid = db_result(db_query("SELECT uid FROM {users} WHERE (mail = '%s')", $email));

    $status = $uid ? 'found' : 'missing';
    $formatted_status = '<span style="color:' . ($uid ? 'green' : 'red') . '">' . $status . '</span>';
    $results[$status]++;

    if ($status_only) {
      if ($split_results) {
        $output[$status][] = array($email, $formatted_status);
      }
      else {
        if ($pass_only && $uid) {
          $output[] = array($email, $formatted_status);
        }
        else if ($fail_only && !$uid) {
          $output[] = array($email, $formatted_status);
        }
        else if (!$fail_only && !$pass_only) {
          $output[] = array($email, $formatted_status);
        }
      }
    }
    else if ($uid && $user = user_load($uid)) {
      profile_load_profile($user);
      if ($dpm) dpm($user);
      $uids[] = $uid;
      $output[] = array(
        l($user->uid, 'user/' . $user->uid . '/edit'),
        $user->name,
        $user->mail,
        implode(',', $user->roles),
        $formatted_status,
      );
    }
    else {
      $output[] = array(
        '',
        '',
        $email,
        '',
        '',
      );
      $uids[] = '';
    }
  }


  if ($status_only) {
    $header = array('mail', 'status',);
  }
  else {
    $header = array('uid', 'name', 'mail', 'roles', 'status',);
  }

  if ($split_results) {
    $data = theme('table', $header, $output['found'], array(), t('Found'));
    $data .= theme('table', $header, $output['missing'], array(), t('Missing'));
  }
  else {
    $data = theme('table', $header, $output);
  }

  $form_state['storage']['mu'] = 'foobar112';
  $form_state['values']['uids'] = implode("\n", $uids);
  $form_state['values']['data'] = $data;
  $form_state['values']['emails'] = implode("\n", $emails);
}
