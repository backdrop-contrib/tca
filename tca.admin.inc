<?php
/**
 * @file
 * Admin page callback for Token Content Access.
 */

/**
 * Token Content Access Global Setting form.
 */
function tca_config_form($form, &$form_state) {
  // Define the form config for retrieving and saving config.
  $tca_config = config('tca.settings');
  $form['#config'] = 'tca.settings';

  // Flood settings fieldset.
  $form['flood_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Flood Control Settings'),
    '#description' => t('Limit failed Token Content Access authentication attempts'),
  );
  // Enable or disable flood control.
  $form['flood_settings']['flood_control_active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable flood control'),
    '#default_value' => $tca_config->get('flood_control_active') ?? TRUE,
  );

  // Limit by IP Address.
  $form['flood_settings']['ip_limit_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Limit by IP address'),
    '#states' => array(
      'visible' => array(
        ':input[name="flood_control_active"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['flood_settings']['ip_limit_settings']['wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['flood_settings']['ip_limit_settings']['wrapper']['flood_ip_limit'] = array(
    '#type' => 'select',
    '#title' => t('Attempted Token Content Access authentication limit'),
    '#title_display' => 'invisible',
    // phpcs:ignore Backdrop.Arrays.Array.LongLineDeclaration -- As each item is just a number, it is more readable in single line.
    '#options' => backdrop_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 75, 100, 125, 150, 200, 250, 500)),
    '#default_value' => $tca_config->get('flood_ip_limit'),
    '#prefix' => t('Limit to'),
  );
  $form['flood_settings']['ip_limit_settings']['wrapper']['flood_ip_window'] = array(
    '#type' => 'select',
    '#title' => t('Attempted Token Content Access authentication time window'),
    '#title_display' => 'invisible',
    // phpcs:ignore Backdrop.Arrays.Array.LongLineDeclaration -- As each item is just a number, it is more readable in single line.
    '#options' => array(0 => t('None (disabled)')) + backdrop_map_assoc(array(60, 180, 300, 600, 900, 1800, 2700, 3600, 10800, 21600, 32400, 43200, 86400), 'format_interval'),
    '#default_value' => $tca_config->get('flood_ip_window'),
    '#prefix' => t('failed authentication attempts from one IP address per'),
  );
  $form['flood_settings']['ip_limit_settings']['help'] = array(
    '#type' => 'item',
    '#description' => t('Do not allow any Token Content Access authentication attempts from the current user\'s IP if the limit has been reached. This is independent of the per-entity and per-Views Display limit to catch attempts from one IP to authenticate against many different Token Content Access protected entities or Views Displays. By default we have a reasonably high limit since there may be only one apparent IP for all users at an institution.'),
  );
  // Limit per entity.
  $form['flood_settings']['entity_limit_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Limit by entity or Views Display'),
    '#states' => array(
      'visible' => array(
        ':input[name="flood_control_active"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['flood_settings']['entity_limit_settings']['wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['flood_settings']['entity_limit_settings']['wrapper']['flood_entity_limit'] = array(
    '#type' => 'select',
    '#title' => t('Failed Token Content Access authentication attempts limit'),
    '#title_display' => 'invisible',
    // phpcs:ignore Backdrop.Arrays.Array.LongLineDeclaration -- As each item is just a number, it is more readable in single line.
    '#options' => backdrop_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 75, 100, 125, 150, 200, 250, 500)),
    '#default_value' => $tca_config->get('flood_entity_limit'),
    '#prefix' => t('Limit to'),
  );
  $form['flood_settings']['entity_limit_settings']['wrapper']['flood_entity_window'] = array(
    '#type' => 'select',
    '#title' => t('Failed Token Content Access authentication attempts time window'),
    '#title_display' => 'invisible',
    // phpcs:ignore Backdrop.Arrays.Array.LongLineDeclaration -- As each item is just a number, it is more readable in single line.
    '#options' => array(0 => t('None (disabled)')) + backdrop_map_assoc(array(60, 180, 300, 600, 900, 1800, 2700, 3600, 10800, 21600, 32400, 43200, 86400), 'format_interval'),
    '#default_value' => $tca_config->get('flood_entity_window'),
    '#prefix' => t('failed authentication attempts on an entity or Views Display per'),
  );
  $form['flood_settings']['entity_limit_settings']['help'] = array(
    '#type' => 'item',
    '#description' => t('Configure the limit and the time window for users attempting to authenticate Token Content Access protected entities or Views Displays. That is, how many failed authentication attempts are allowed per specified time period.'),
  );
  // Define whether should fail silently once flood control threshold is
  // reached or display a message.
  $form['flood_settings']['flood_control_silent'] = array(
    '#type' => 'checkbox',
    '#title' => t('Silently block when flood control threshold reached'),
    '#default_value' => $tca_config->get('flood_control_silent') ?? FALSE,
    '#description' => t("Backdrop displays a message when a flood control limit is reached for user logins, so the default here is to display a message. However, you may prefer it to fail silently so attackers aren't alerted that they are being blocked."),
  );

  // Parameter key fieldset.
  $form['parameter_key_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Parameter Key Settings'),
    '#description' => t("The default URL parameter key is 'tca' (e.g. https://example.com/about?<strong>tca</strong>=token-value). You could change it to provide a better fit with the use case on your site or simply to make it harder for any brute force attempts."),
  );
  $form['parameter_key_settings']['default_parameter_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Set the default parameter key for your site.'),
    '#default_value' => $tca_config->get('default_parameter_key') ?? 'tca',
    '#element_validate' => array('tca_parameter_key_validate'),
    '#required' => TRUE,
  );
  $form = system_settings_form($form);

  return $form;
}
