<?php
/**
 * @file
 * Add setting to view to override the Token Content Access restriction.
 */

/**
 * Class for above.
 */
class tca_node_views_plugin_display_extender extends views_plugin_display_extender {

  /**
   * Default values.
   */
  public function options_definition() {
    $options = parent::option_definition();
    $options['tca_node_override'] = array('default' => 0);
    return $options;
  }

  /**
   * Provide a form to edit options for this plugin.
   */
  public function options_definition_alter(&$options) {
    $options['tca_node_override'] = array(
      'default' => 0,
    );
  }

  /**
   * Provide a form to edit options for this plugin.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    if ($form_state['section'] === 'tca_node') {
      $form['tca_node_override'] = array(
        '#type' => 'checkbox',
        '#title' => t('Token Content Access Node Override'),
        '#description' => t('Token Content Access Node restricts access to views if you don\'t have the permission to bypass. This is mostly desirable but there may be some views you wish to enable, for example, a view of contextual information in a block on the node. Use with caution.'),
        '#default_value' => $this->display->get_option('tca_node_override'),
      );
    }
  }

  /**
   * Handle any special handling on the validate form.
   */
  public function options_submit(&$form, &$form_state) {
    if (isset($form_state['values']['tca_node_override'])) {
      $this->display->set_option('tca_node_override', $form_state['values']['tca_node_override']);
    }
  }

  /**
   * Provide the default summary for options in the views UI.
   *
   * This output is returned as an array.
   */
  public function options_summary(&$categories, &$options) {
    $options['tca_node'] = array(
      'category' => 'other',
      'title' => t('Token Content Access Node Override'),
      'value' => ($this->display->get_option('tca_node_override')) ? 'Yes' : 'No',
      'desc' => t('Override the Token Content Access Node restrictions for this view display.'),
    );
  }
}
