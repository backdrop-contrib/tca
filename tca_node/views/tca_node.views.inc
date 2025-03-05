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
  if (isset($view->filter['type'])) {
    // If it is get the filter value(s).
    $filter_content_type = $view->filter['type']->value;
  }
  else {
    // If it isn't, generate a comparable list of all node types.
    $filter_content_type = array_keys(node_type_get_types());
  }

  // Loop through each node type and check if the user has the bypass tca_node
  // permission for that type.
  $type_bypass_permissions = array();
  foreach ($filter_content_type as $type) {
    $type_bypass_permissions[$type] = (bool) user_access('bypass tca_node ' . $type);
  }
  $count_permissions = array_sum($type_bypass_permissions);
  // Add all the following conditions to an OR group within WHERE.
  $query->set_where_group('OR', 'tca');
  if ($count_permissions == count($filter_content_type)) {
    // If bypass permission exists for all content types for user then no alter
    // is needed.
    return;
  }
  else {
    // Add a counter to ensure that query placeholders are unique for each
    // content type.
    $i = 0;
    // If some bypass permissions exist then prepare a statement for each
    // content type.
    foreach ($type_bypass_permissions as $type => $bypass) {
      // Ensure placeholders are unique for each type.
      $placeholder = ":type_$i";
      if ($bypass || !tca_get_active_bundle('node', $type)) {
        // If bypass permission or content type not activated for Token Content
        // Access then add simple statement for type.
        $query->add_where_expression('tca', "node.type = $placeholder", array($placeholder => $type));
      }
      else {
        // If no bypass permission and content type is activated for Token
        // Content Access then add statement for type and check for tca_active
        // status.
        $query->add_where_expression('tca', "node.type = $placeholder AND (node.tca_active = '0' OR node.tca_active IS NULL)", array($placeholder => $type));
      }
      // Increment the counter.
      $i++;
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
    ),
  );

  return $plugins;
}
