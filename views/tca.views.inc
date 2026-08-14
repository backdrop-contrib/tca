<?php
/**
 * @file
 * Provide views data and handlers for Token Content Access module.
 */

/**
 * Implements hook_views_query_alter().
 */
function tca_views_query_alter(&$view, &$query) {
  $display = $view->display[$view->current_display];
  $display_type = $display->display_plugin;
  $tca_views_protect = $display->display_options['tca_views_protect'] ?? FALSE;
  $view_bypass_permission = user_access('bypass tca views protect');

  if ($tca_views_protect && !$view_bypass_permission) {
    if ($display->display_options['tca_set_method'] == 'settings') {
      $token_setting_key = 'tca.views.' . $view->name . '.' . $view->current_display . '.token';
      $view_token = settings_get($token_setting_key);
      if (is_null($view_token)) {
        // $view_token = $display->display_options['tca_token'];
        watchdog(
          'Token Content Access',
          t('Views display (@view_display) has been set to use a token defined in settings.php but the setting cannot be found.'),
          array('@view_display' => $view->name . '.' . $view->current_display),
          WATCHDOG_ERROR,
          l(t('Edit the View Display'), 'admin/structure/views/view/' . $view->name . '/configure/' . $view->current_display)
        );
        return;
      }
    }
    else {
      $view_token = $display->display_options['tca_token'];
    }

    // Get the TCA config for the flood control and parameter key settings.
    $tca_config = config_get('tca.settings');
    $flood_control_active = (bool) $tca_config['flood_control_active'];

    if ($flood_control_active) {
      $flood_allowed_ip = flood_is_allowed('tca_failed_authentication_attempt_ip', $tca_config['flood_ip_limit'], $tca_config['flood_ip_window']);
      $flood_allowed_view = flood_is_allowed('tca_failed_authentication_attempt_view', $tca_config['flood_entity_limit'], $tca_config['flood_entity_window'], $view->name . '-' . $view->current_display);
      if (!$flood_allowed_ip || !$flood_allowed_view) {
        if (!$flood_allowed_ip) {
          $message = t('Sorry, too many failed authentication attempts from your IP address. This IP address is temporarily blocked. Try again later.');
        }
        if (!$flood_allowed_view) {
          $message = t('Sorry, too many failed authentication attempts for this View. Try again later.');
        }
        if (!$tca_config['flood_control_silent']) {
          backdrop_set_message($message, 'error');
        }
        $view_access = FALSE;
      }
    }

    // Get query parameters.
    $url_query = backdrop_get_query_parameters();

    // Get the parameter key setting.
    $key = $display->display_options['tca_parameter_key'] ?? $tca_config['default_parameter_key'];

    // Check to see if a token was provided via the URL.
    if (isset($url_query[$key])) {
      $user_token = $url_query[$key];
      if (hash_equals($view_token, $user_token)) {
        // If the token matches, then set a session variable and proceed with
        // viewing the view.
        if (!backdrop_session_started()) {
          // If the user is anonymous, start a session so the token will
          // persist.
          backdrop_session_start();
        }
        // Store a special token based on the user session, the TCA token and
        // the site's private key.
        $_SESSION['tca_token'] = backdrop_get_token($view_token);
        // Clear view event if flood control if set.
        if ($tca_config['flood_control_active']) {
          flood_clear_event('tca_failed_authentication_attempt_view', $view->name . '-' . $view->current_display);
        }
        // Proceed with viewing the view.
        $view_access = TRUE;
      }
      else {
        // If the token doesn't match:
        // Apply flood control if set.
        if ($tca_config['flood_control_active']) {
          flood_register_event('tca_failed_authentication_attempt_ip', $tca_config['flood_ip_window']);
          flood_register_event('tca_failed_authentication_attempt_view', $tca_config['flood_entity_window'], $view->name . '-' . $view->current_display);
        }
        // Clear the session token if it exists.
        unset($_SESSION['tca_token']);
        // Set to deny access to the View.
        $view_access = FALSE;
      }
    }
    else {
      // If a token hasn't been provided check for a session token.
      $session_token = $_SESSION['tca_token'] ?? FALSE;
      if ($session_token && backdrop_valid_token($session_token, $view_token)) {
        // If token matches:
        // Clear view event if flood control if set.
        if ($tca_config['flood_control_active']) {
          flood_clear_event('tca_failed_authentication_attempt_view', $view->name . '-' . $view->current_display);
        }
        // Proceed with viewing the view.
        $view_access = TRUE;
      }
      else {
        // Clear the session token if it exists.
        unset($_SESSION['tca_token']);
        // Apply flood control if set.
        if ($tca_config['flood_control_active']) {
          flood_register_event('tca_failed_authentication_attempt_ip', $tca_config['flood_ip_window']);
          flood_register_event('tca_failed_authentication_attempt_view', $tca_config['flood_entity_window'], $view->name . '-' . $view->current_display);
        }
        // If the session token doesn't exist or doesn't match, deny access.
        $view_access = FALSE;
      }
    }

    if (!$view_access) {
      switch ($display_type) {
        case 'page':
          backdrop_access_denied();
          backdrop_exit();
          break;
        case 'block':
          // Add a where expression that will always be FALSE and therefore
          // prevent any rows from being returned.
          $query->add_where_expression(0, 'Night' == 'Day');
          break;
        default:
          // Do nothing.
      }
    }
    else {
      return;
    }
  }
}

/**
* Implements hook_views_plugins().
 */
function tca_views_plugins() {
  $path = backdrop_get_path('module', 'tca');
  $plugins = array();
  $plugins['display_extender'] = array(
    'tca' => array(
      'title' => t('Token Content Access View Protection'),
      'help' => t('Token Content Access restricts access to views if you don\'t have the permission to bypass, or don\'t have the token in the URL or in the session.'),
      'path' => $path,
      'handler' => 'tca_views_plugin_display_extender',
    ),
  );
  return $plugins;
}
