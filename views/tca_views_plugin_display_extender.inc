<?php
/**
 * @file
 * Add setting to view to provide generic Token Content Access restriction to
 * Views.
 */

/**
 * Class for above.
 */
class tca_views_plugin_display_extender extends views_plugin_display_extender {

  /**
   * Default values.
   */
  public function options_definition() {
    $options = parent::option_definition();
    $options['tca_views_protect'] = array(
      'default' => FALSE,
      'bool' => TRUE,
    );
    $option['tca_parameter_key'] = array(
      'default' => '',
    );
    $options['tca_set_method'] = array(
      'default' => '',
    );
    $options['tca_token'] = array(
      'default' => '',
    );
    return $options;
  }

  /**
   * Provide a form to edit options for this plugin.
   */
  public function options_definition_alter(&$options) {
    $options['tca_views_protect'] = array(
      'default' => FALSE,
      'bool' => TRUE,
    );
    $option['tca_parameter_key'] = array(
      'default' => '',
    );
    $options['tca_set_method'] = array(
      'default' => '',
    );
    $options['tca_token'] = array(
      'default' => '',
    );
  }

  /**
   * Provide a form to edit options for this plugin.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    if ($form_state['section'] === 'tca') {
      $form_permission = user_access('administer tca views protect');
      $form['tca_views_protect_permission'] = array(
        '#type' => 'hidden',
        '#value' => $form_permission,
      );
      $form['tca_views_protect'] = array(
        '#type' => 'checkbox',
        '#title' => t('Token Content Access Views Protection'),
        '#description' => t('Token Content Access restricts access to views if you don\'t have the permission to bypass, or don\'t have the token in the URL.'),
        '#default_value' => $this->display->get_option('tca_views_protect') ?? FALSE,
        '#disabled' => !$form_permission,
      );
      // If views_protect is enabled then show the settings.
      $form['tca_settings'] = array(
        '#type' => 'fieldset',
        '#collapsed' => !$form_permission,
        '#states' => array(
          'visible' => array(
            ':input[name="tca_views_protect"]' => array('checked' => TRUE),
            ':input[name="tca_views_protect_permission"]' => array('value' => 1),
          ),
        ),
      );
      $site_default_parameter_key = config_get('tca.settings', 'default_parameter_key');
      $form['tca_settings']['tca_parameter_key'] = array(
        '#type' => 'textfield',
        '#title' => t('Set the parameter key for this View Display.'),
        '#default_value' => $this->display->get_option('tca_parameter_key') ?? $site_default_parameter_key,
        '#element_validate' => array('tca_parameter_key_validate'),
        '#required' => TRUE,
      );
      $form['tca_settings']['tca_set_method'] = array(
        '#type' => 'radios',
        '#title' => t('Set your preference for how to add the token to the View'),
        '#options' => array(
          'manual' => t('Manually set the token.'),
          'generate' => t('Generate unique token.'),
        ),
        '#default_value' => $this->display->get_option('tca_set_method'),
        '#required' => TRUE,
        '#states' => array(
          'visible' => array(
            ':input[name="tca_views_protect"]' => array('checked' => TRUE),
          ),
        ),
      );
      $help_text = '<ul><li>';
      $help_text .= t("If 'Generate unique token' is selected, the token will be generated or regenerated when you click 'Apply'.");
      $help_text .= "</li><li>" . t("If you have just opened the form to view and copy the token, click 'Cancel' or the close dialog button once you have finished to avoid regenerating the token.");
      $help_text .= "</li></ul>";
      $form['tca_settings']['tca_token'] = array(
        '#type' => 'textfield',
        '#title' => t('Enter a token'),
        '#default_value' => $this->display->get_option('tca_token'),
        '#description' => $help_text,
        '#element_validate' => array('tca_manual_token_validate'),
        '#states' => array(
          'visible' => array(
            ':input[name="tca_views_protect"]' => array('checked' => TRUE),
          ),
          'required' => array(
            ':input[name="tca_set_method"]' => array('value' => 'manual'),
          ),
          'enabled' => array(
            ':input[name="tca_set_method"]' => array('value' => 'manual'),
          ),
        ),
      );
    }
  }

  /**
   * Ensure that if the token set method is set to manual, a token has been
   * entered.
   */
  public function options_validate(&$form, &$form_state) {
    $form_values = $form_state['values'];
    // Ensure that if manual is selected, then a token is entered.
    if (isset($form_values['tca_set_method']) && $form_values['tca_set_method'] == 'manual' && empty($form_values['tca_token'])) {
      form_set_error('tca_token', t('As you have selected to manually set the token, you must enter a token.'));
    }
  }

  /**
   * Handle any special handling on the validate form.
   */
  public function options_submit(&$form, &$form_state) {
    $tca_views_protect_permission = $form_state['values']['tca_views_protect_permission'] ?? FALSE;
    if ($tca_views_protect_permission) {
      // Only save changes if user has permission to administer this protection.
      if (isset($form_state['values']['tca_views_protect'])) {
        $tca_views_protect = $form_state['values']['tca_views_protect'];
        $this->display->set_option('tca_views_protect', $tca_views_protect);
        if ($tca_views_protect) {
          $tca_set_method = $form_state['values']['tca_set_method'];
          $this->display->set_option('tca_set_method', $tca_set_method);
          $tca_parameter_key = $form_state['values']['tca_parameter_key'];
          $this->display->set_option('tca_parameter_key', $tca_parameter_key);
          switch ($tca_set_method) {
            case 'manual':
              $token = $form_state['values']['tca_token'];
              break;
            case 'generate':
              $view_id = $this->view->name;
              $display_id = $this->view->current_display;
              $token = backdrop_hmac_base64($view_id, $view_id . $display_id . microtime() . backdrop_get_private_key() . backdrop_get_hash_salt());
              break;
          }
          $this->display->set_option('tca_token', $token);
        }
        else {
          // Set the set method and token to default values.
          $this->display->set_option('tca_parameter_key', '');
          $this->display->set_option('tca_set_method', '');
          $this->display->set_option('tca_token', '');
        }
      }
    }

  }

  /**
   * Provide the default summary for options in the views UI.
   *
   * This output is returned as an array.
   */
  public function options_summary(&$categories, &$options) {
    // Add a category.
    $categories['token_content_access'] = array(
      'title' => t('Token Content Access'),
      'column' => 'second',
      'build' => array(
        '#weight' => -4,
      ),
    );
    $protect = $this->display->get_option('tca_views_protect');
    $value = $protect ? 'Yes (View)' : 'No';

    $options['tca'] = array(
      'category' => 'token_content_access',
      'title' => t('Token Content Access View Protection'),
      'value' => $value,
      'desc' => t('Token Content Access restricts access to views if you don\'t have the permission to bypass, or don\'t have the token in the URL.'),
    );
  }
}
