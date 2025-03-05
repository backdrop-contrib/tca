# Token Content Access
<!--
The first paragraph of this file should be kept short as it will be used as the
project summary on BackdropCMS.org. Aim for about 240 characters (three lines at
80 characters each).

All lines in this file should be no more than 80 characters long for legibility,
unless including a URL or example that requires the line to not wrap.
|<- - - - - - - This line is exactly 80 characters for reference - - - - - - ->|

Detail in READMEs should be limited to the minimum required for installation and
getting started. More detailed documentation should be moved to a GitHub wiki
page; for example: https://github.com/backdrop-contrib/setup/wiki/Documentation.
-->
Token Content Access allows you to restrict access to individual entities using
URL tokens. In order to view protected entities, users must provide a unique
token via the URL.

This allows entities to be published and viewable to anonymous users (for
instance with a special link from an email campaign) but not visible to the
public at large.

Permissions are provided to administer each entity based module and to bypass
the protection for all entities in that module and for bundles (e.g. Content
Types for Token Content Access Nodes).

Node protection is available in the included Token Content Access Nodes
(`tca_node`) submodule and this can be used as a model to extend to other
entities such as taxonomy terms, users, files and custom entities. It protects
access to the node view page and also removes from Views, though that can be
overridden on particular Views Displays.  The token and protection status are
available to include in a View, which supports both the use of contextual
filters and being able to display the URL with token so an authorised user can
copy it to share.

This module is designed with performance in mind, so it doesn't use traditional
solutions like node grants. This also means that it's not guaranteed to block
access in all situations, for instance, if you expose node content via means
other than Views.

## Installation
<!--
List the steps needed to install and configure the module. Add/remove steps as
necessary.
-->
- Install this module using the official Backdrop CMS instructions at
  https://docs.backdropcms.org/documentation/extend-with-modules.
- Also install the 'Token Content Access Nodes' module.

## Usage
### Token Content Access Nodes
Configurable per Content Type on the "Token Content Access settings" tab. To
enable, just check "Enable Token Content Access protection" on the content
type Configuration screen and then you will see the "Token Content Access
settings" tab on the individual node editing page. You can also force the
protection to be applied to every node in that content type; when this setting
is enabled, this will apply to new nodes and when an existing node is edited.

By default Nodes with Token Content Access protection will be excluded from
Views but you can override that with the "Token Content Access Node Override"
setting, which is applied to each display within the View.

## Issues
<!--
Link to the repo's issue queue.
-->
Bugs and Feature Requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/tca/issues.

## Current Maintainers
<!--
List the current maintainer(s) of the module, and note if this module needs
new/additional maintainers.
-->
- [Martin Price](https://github.com/yorkshire-pudding) - [System Horizons Ltd](https://www.systemhorizons.co.uk)
- Collaboration and co-maintainers welcome!

## Credits
<!--
Give credit where credit's due.
If this is a Drupal port, state who ported it, and who wrote the original Drupal
module. If this module is based on another project, or uses third-party
libraries, list them here. You can also mention any organisations/companies who
sponsored the module's development.
-->
- Ported to Backdrop CMS by - [Martin Price](https://github.com/yorkshire-pudding) - [System Horizons Ltd](https://www.systemhorizons.co.uk).
- Port and ongoing development sponsored by [System Horizons Ltd](https://www.systemhorizons.co.uk).
- Originally written for Drupal by [Grant Gaudet](https://www.drupal.org/u/gg4) and [Dane Powell](https://www.drupal.org/u/dane-powell)

## License
<!--
Mention what license this module is released under, and where people can find
it.
-->

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.