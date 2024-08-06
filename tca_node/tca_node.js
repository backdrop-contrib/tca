(function($) {
  Backdrop.behaviors.tcaNode = {
    attach: function (context) {
      var $context = $(context);
      var summary = '';
      var enabled = Backdrop.t('Enabled');
      var disabled = Backdrop.t('Disabled');

      $context.find('fieldset#edit-tca').backdropSetSummary(function() {
        // Set the summary for the node form.
        if ($context.find('input[name="tca_active"]').length) {
          if ($context.find('input[name="tca_active"]:checked').length) {
            summary = enabled;
          }
          else {
            summary = disabled;
          }
        }
        // Set the summary for the node type settings form.
        if ($context.find('input[name="tca_node_active"]').length) {
          if ($context.find('input[name="tca_node_active"]:checked').length) {
            summary = enabled;
          }
          else {
            summary = disabled;
          }
        }
        return summary;
      });
    }
  };

})(jQuery);
