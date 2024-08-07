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
  );
}

/**
 * Implements hook_views_query_alter().
 */
function tca_node_views_query_alter(&$view, &$query) {
  // Get the current display.
  $display = $view->current_display;

  // Set the option to retrieve.
  $option = 'tca_node_override';

  // Check if the override applies to this view.
  if (isset($view->display[$display]->display_options[$option])) {
    $view_override = (bool) $view->display[$display]->display_options[$option];
  }
  else {
    $view_override = FALSE;
  }

  // Check if the user has permission to bypass the protection.
  $bypass_permission = (bool) user_access('bypass tca_node');

  // Confirm the base table is a node.
  $node_base = (bool) $query->base_table == 'node';

  // Alter all node queries to remove TCA protected nodes if user does not have
  // bypass access and the override option is not enabled on the view.
  if ($node_base && !$bypass_permission && !$view_override) {
    $query->add_field('node', 'tca_active', 'tca_active');
    $query->add_where_expression(0, "node.tca_active = '0' OR node.tca_active IS NULL");
  }
}

/**
* Implements hook_views_plugins().
 */
function tca_node_views_plugins() {
  $path = backdrop_get_path('module', 'tca_node');
  $plugins = array();

  $plugins['display_extender'] = array(
    'mymodule' => array(
      'title' => t('Some Setting'),
      'help' => t('A description of the setting.'),
      'path' => $path,
      'handler' => 'tca_node_views_plugin_display_extender',
    ),
  );

  return $plugins;
}
