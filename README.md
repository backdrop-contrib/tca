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
manually set or unique generated tokens. In order to view protected entities,
users must provide a matching token via the URL.

This allows entities to be published and viewable to anonymous users (for
instance with a special link from an email campaign) but not visible to the
public at large.

Views can also be protected using a token; for Views Page display, then access
is denied, but for other display types, the rows are removed so it returns an
empty result.

Permissions are provided to administer the module and each entity based module
and to bypass the protection for all entities in that module and for bundles
(e.g. Content Types for Token Content Access Nodes).

## Submodule(s)
### Token Content Access Nodes
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
other than Views such as using the "Existing content" block in a layout or
in an Entity Reference field.

## Differences from Drupal 7
- Support token and TCA status in Views
- Include an override in Views displays to include protected nodes in a
display.
- Support protection of Views.
- Add bypass permissions per bundle (e.g. per Content Type for Nodes) and for
Views.
- Allow token usage to be enforced per bundle
- Allow token to be overriden manually so you can set the same as another
entity.
- Allow parameter key to be configured for the site and per bundle and View.
- Store the token in the session so user can access other nodes or Views using
the same token without having to re-enter the URL parameter.
- Copy the token from another node with a token.
- Copy the token from another part of the same Book (top of book or parent menu
item).
- Use Views Bulk Operations to activate and manually add or generate a token,
and to deactivate TCA Node from nodes.
- Optional and configurable flood control to provide additional protection
against attempts to brute force the token.

Most of these additional features are also not available in Drupal 8+.

## Installation
<!--
List the steps needed to install and configure the module. Add/remove steps as
necessary.
-->
- Install this module using the official Backdrop CMS instructions at
  https://docs.backdropcms.org/documentation/extend-with-modules.
- Also enable the 'Token Content Access Nodes' module.

## Usage
For documentation on how to use the module including advanced usage, see the
documentation pages at https://docs.backdropcms.org/documentation/add-ons/token-content-access.

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