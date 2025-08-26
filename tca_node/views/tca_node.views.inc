<?php
/**
 * @file
 * Provide views data and handlers for Token Content Access Nodes module.
 */

/**
 * Implements hook_views_data_alter().
 */
function tca_node_views_data_alter(&$data) {
  $data['node']['tca_active'] = array(
    'title' => t('Token Content Access Node Status'),
    'help' => t('Status of Token Content Access (TCA) for Nodes'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Include nodes with Token Content Access (TCA)'),
      'type' => 'Enabled-Disabled',
      'use equal' => TRUE,
      'accept null' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['node']['tca_token'] = array(
    'title' => t('Token Content Access Token'),
    'help' => t('Token for Token Content Access (TCA) Nodes'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => FALSE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
}

/**
 * Implements hook_views_query_alter().
 */
function tca_node_views_query_alter(&$view, &$query) {
  // If the base table is not a node then no alter is needed.
  if ($query->base_table != 'node') {
    return;
  }

  // If the user has permission to bypass protection for all node types then no
  // alter is needed.
  if (user_access('bypass tca_node')) {
    return;
  }

  // If the display has the Token Content Access Override option enabled then
  // no alter is needed.
  // Get the current display.
  $display = $view->current_display;

  // Set the option to retrieve.
  $option = 'tca_node_override';

  // If the override applies to this view then no alter is needed.
  if (isset($view->display[$display]->display_options[$option])) {
    if ((bool) $view->display[$display]->display_options[$option]) {
      return;
    }
  }

  // Check if the view is filtered by content type.
  // Loop through all the filter keys for any starting with 'type'.
  $filter_keys = array_keys($view->filter);
  foreach ($filter_keys as $index => $key) {
    if (strpos($key, 'type') !== 0) {
      // Remove any that don't start with 'type'.
      unset($filter_keys[$index]);
    }
  }
  if (!empty($filter_keys)) {
    // If there is at least one filter with 'type' in, get any included values.
    $filter_content_type = array();
    foreach ($filter_keys as $key) {
      $filter = $view->filter[$key];
      if ($filter->operator == 'in') {
        $filter_content_type = array_merge($filter_content_type, $filter->value);
      }
    }
  }
  else {
    // If the view isn't filtered by content type, generate a comparable list of
    // all node types.
    $filter_content_type = array_keys(node_type_get_types());
  }

  // Loop through each node type.
  $type_bypass_permissions = array();
  $type_tca_active = array();
  foreach ($filter_content_type as $type) {
    // Check if the type is active.
    $type_tca_active[$type] = (bool) tca_get_active_bundle('node', $type);
    // Check if the user has the bypass tca_node permission for that type.
    $type_bypass_permissions[$type] = (bool) user_access('bypass tca_node ' . $type);
  }

  // Count how node types are active.
  $count_tca_active = array_sum($type_tca_active);
  // If none of the node types are active for TCA then no alter is needed.
  if ($count_tca_active == 0) {
    return;
  }

  $count_permissions = array_sum($type_bypass_permissions);
  if ($count_permissions == count($filter_content_type)) {
    // If bypass permission exists for all content types for user then no alter
    // is needed.
    return;
  }
  else {
    // Loop through each type and:
    // - if the bypass permission exists or the type is not TCA active, add to
    //   $pass_types array.
    // - if the bypass permission does not exist and the type is TCA active,
    //   add to the $check_types array.
    $pass_types = array();
    $check_types = array();
    foreach ($type_bypass_permissions as $type => $bypass) {
      if ($bypass || !tca_get_active_bundle('node', $type)) {
        $pass_types[] = $type;
      }
      else {
        $check_types[] = $type;
      }
    }
    // Define the WHERE expression snippet.
    // If type is pass_types then no other checks needed.
    $snippet = "(node.type IN (:pass_types))";
    $snippet .= " OR ";
    // If type in check_types then only include nodes where TCA is not active.
    $snippet .= "(node.type IN (:check_types) AND ";
    $snippet .= "(node.tca_active = '0' OR node.tca_active IS NULL))";
    // Define the placeholders using the relevant array.
    $placeholders = array(
      ':pass_types' => $pass_types,
      ':check_types' => $check_types,
    );
    // Determine where to put the expression in relation to existing WHERE
    // groups.
    $where_group_count = count($query->where);
    $target_groups = array();
    switch ($where_group_count) {
      case 0:
        $target_groups[] = 'tca';
        break;
      case 1:
        foreach ($query->where as $key => $where_group) {
          if ($where_group['type'] == 'AND') {
            $target_groups[] = $key;
          }
          else {
            $target_groups[] = 'tca';
          }
        }
        break;
      default:
        $operator = $query->group_operator;
        if ($operator == 'AND') {
          $target_groups[] = 'tca';
        }
        else {
          // Group operator is 'OR'.
          foreach ($query->where as $key => $where_group) {
            if ($where_group['type'] == 'AND') {
              $target_groups[] = $key;
            }
            else {
              // With 'OR' group operator and OR type, there is nowhere the
              // expression can be added. This scenario, however is unlikely as
              // there is no need to have seperate groups; all the OR conditions
              // can be part of a single group.
            }
          }
        }
    }
    $target_groups = array_unique($target_groups);
    foreach ($target_groups as $group) {
      $query->add_where_expression($group, $snippet, $placeholders);
    }
  }
}

/**
* Implements hook_views_plugins().
 */
function tca_node_views_plugins() {
  $path = backdrop_get_path('module', 'tca_node');
  $plugins = array();

  $plugins['display_extender'] = array(
    'tca_node' => array(
      'title' => t('Token Content Access Node Override'),
      'help' => t('Token Content Access Node restricts access to views if you don\'t have the permission to bypass. This is mostly desirable but there may be some views you wish to enable, for example, a view of contextual information in a block on the node. Use with caution.'),
      'path' => $path,
      'handler' => 'tca_node_views_plugin_display_extender',
      'enabled' => TRUE,
    ),
  );

  return $plugins;
}
