
# Posts Module Change Log

## [1.34.25] - 2024-10-17

- Version bump.

## [1.34.23] - 2024-08-20

- Added offending words list to the SQL injection detection notification on saving.

## [1.34.22] - 2023-07-01

- Added GET var to allow full content instead of excerpts on RSS feed calls.

## [1.34.21] - 2023-04-16

- Added check to avoid warning on the posts repository.

## [1.34.20] - 2022-11-07

- Added checks for dodging encoding issues in the JSON posts deliverer.

## [1.34.19] - 2022-09-24

- Added checks against script injections.

## [1.34.18] - 2022-05-04

- Added input sanitization on getter of the repository.

## [1.34.17] - 2022-03-16

- Refactored IP Geolocation functions.

## [1.34.16] - 2022-01-05

- Added attributes to the `render_post_cards` shortcode.

## [1.34.15] - 2021-12-31

- Input sanitization on the "search by tag" script.

## [1.34.14] - 2021-12-31

- Input sanitization on the "search by tag" builder extender.

## [1.34.13] - 2021-12-17

- Input sanitization on the posts repository.

## [1.34.12] - 2021-12-17

- Input sanitization on the posts repository.

## [1.34.11] - 2021-12-13

- Added SQL injection checks.

## [1.34.10] - 2021-11-17

- Added SQL injection check.

## [1.34.9] - 2021-10-25

- Tuned enforced expiration detection on save.

## [1.34.8] - 2021-07-27

- Added `render_post_cards` shortcode handler.

## [1.34.7] - 2021-07-20

- Added extension point on the posts saving script.

## [1.34.6] - 2021-07-05

- Added check to avoid generating numeric slugs.

## [1.34.5] - 2021-02-27

- Added identifiers to form segments for easing JS/CSS manipulation.

## [1.34.4] - 2021-01-25

- Allowed multiple category ids in the "posts by category" widget.

## [1.34.3] - 2020-04-28

- Removed "since" parameter when invalid on record navigation injection to post feeds.
- Added posts by tag feed.
- Added SQL injection check on post feeds.
- Added UTF-8 encoding check on post feeds to avoid fatal errors.

## [1.34.2] - 2020-04-22

- Implemented record navigation injection to post feeds.

## [1.34.1] - 2019-12-08

- Added login form for unauthenticated users accessing the records browser.

## [1.34.0] - 2019-10-08

- Added support for full contents output on RSS feeds.
- Changed RSS author field from user name to display name.
- Changed RSS links to settings based instead of post ids.

## [1.33.1] - 2019-07-27

- Fixed issue in slug generator.

## [1.33.0] - 2019-07-16

- Added checks for incidental bogus post ids.
- Moved rendering of the quick post button for mobiles to the pre-EOF area.
- Added option to show/hide the quick post button for mobiles on the settings page.

## [1.32.0] - 2019-06-30

- Added extension point on the quick post form.
- Added extension point after setting metas on saving.

## [1.31.2] - 2019-06-06

- Tuned user profile home sections.

## [1.31.1] - 2019-06-01

- Added missing limits on the popular posts widget.

## [1.31.0] - 2019-04-25

- Added optional limits to slider and featured posts.
- Tuned post fields on the settings editor.

## [1.30.1] - 2019-03-19

- Added support for `og:video` injection over the first video on the contents if available.

## [1.30.0] - 2019-03-11

- Added filtering that caused empty root feeds.
- Added category listing widget for the right sidebar.
- Fixed permalink issues on all widgets.
- Fixed wrong ordering issues.

## [1.29.1] - 2019-02-01

- Fixed typo in `post_content` shortcode file handler.
- Added missing case in `post_content` shortcode converter.

## [1.29.0] - 2019-02-01

- Added shortcodes.

## [1.28.6] - 2018-12-11

- Added filter for non-public posts on categories RSS feed.

## [1.28.5] - 2018-09-07

- Refactored logic to add meta image.
- Added "data-main-category-slug" body attribute to single posts.

## [1.28.4] - 2018-07-15

- Language fixes.

## [1.28.3] - 2018-03-19

- Passed BCM version on web requests.

## [1.28.2] - 2017-12-18

- Added extra check for empty contents on saving.

## [1.28.1] - 2017-12-14

- Added check to avoid a warning when fetching the category selector data on the composition forms.
- Added extension point.

## [1.28.0] - 2017-12-14

- Prevented showing the quick post composition link on "popup" layouts.
- Added flag to avoid triklet-based redirections when available.
- Added support for externalized quick posts form (on an iframe).
- Added support for full external posts edition (Mobile platform).

## [1.27.1] - 2017-11-18

- Added required module details for settings with external requirements.

## [1.27.0] - 2017-09-01

- Added filtering by scheduled/expired posts on the posts browser.
- Added publishing date changer for scheduled posts straight on the posts browser.
- Added "light" view to the posts browser.
- Added check to avoid loading the home slider posts through a template var injection.

## [1.26.3] - 2017-08-19

- Improved records browser navigation.
- Simplified publishing date editor.

## [1.26.2] - 2017-08-11

- Version bump.

## [1.26.1] - 2017-08-11

- Added settings to control autobumping of index caches.
- Added a fix that threw an exception when autosaving drafts on a cluster with sync delays.
- Improved publishing date editor.

## [1.26.0] - 2017-08-11

- Added main index caching for users (by level) with automatic bumping.
- Fixed issue in posts archive by date that allowed showing future posts.
- Allowed editing of publishing date on the composition form.

## [1.25.3] - 2017-07-29

- Added check on the posts repository to allow extenders avoiding unneeded data preloads.

## [1.25.2] - 2017-07-26

- Added extension point on the Quick Posts form.
- Added check on the Quick Posts form to allow hiding it based on the exceptions
  specified in the main category selector.
- Added helper method the posts repository.
- Fixed a potential issue on the post_record method used to store custom fields.

## [1.25.1] - 2017-07-19

- Fixed issue in post_meta table creation.

## [1.25.0] - 2017-07-18

- Added support for post metas and custom fields editor.

## [1.24.0] - 2017-07-11

- Hidden pagination helpers for single-page post indexes.
- Added checks on the category index to allow full content overrides by extenders.
- Added info of categories excluded from the main index on the categories browser.
- Added extension point on the quick posts form and renamed cache keys to allow purging.
- Added extension point on the posts browser and JS function extender support for the post category selector.
- Adjustments to the posts repository.
- Fixed wrong display of parent info when editing orphan posts.
- Added option to allow posts editing always.
- Added option to specify user level allowed to edit posts with comments.

## [1.23.0] - 2017-07-03

- Increased security against disabled accounts.
- Added og:type meta tag.
- Added support for child posts:
  - Automatic trees are inserted at the top of single post contents.
  - Trees are fully responsive.
  - Shortcode added to allow relocation and customization.

## [1.22.2] - 2017-06-22

- Fixed wrong date comparison in posts check against deletion.

## [1.22.1] - 2017-06-16

- Added helper to remove posts by tag widget entries without needing to edit the posts.

## [1.22.0] - 2017-06-16

- Added extension points on post indexes by category, tag, date and author.
- Added JS extension point for the category selector on the post composition forms.
- Improved object caching.
- Other minor improvements and cleanup.

## [1.21.3] - 2017-06-08

- Added extension point on posts_repository class.

## [1.21.2] - 2017-05-29

- Added missing restrictions to post on a category limited by user level.

## [1.21.1] - 2017-05-25

- Allowed users with level 100 and up to delete their own posts with comments.

## [1.21.0] - 2017-05-20

- Added upload progress indicator to quick post form.
- Added missing parameter for Triklet redirector.

## [1.20.1] - 2017-05-16

- Added extension point to the browser.

## [1.20.0] - 2017-05-13

- Updated widgets to support new widgets manager version.
  **WARNING:** After updating to this version, all existing widgets must be reviewed.

## [1.19.0] - 2017-04-23

- Changed the way post passwords are set to avoid unwanted autofills.
- Improved editor/mod controls on composition form.
- Added extension point ons the editor form.

## [1.18.0] - 2017-04-23

- Polished widgets.
- Fixes and improvements on the repository class.
- Added user home section with post counts per category (with charts for moderators).
- Added recent posts chart links on the accounts browser, posts browser and several other places for quick viewing.
- Added meta tags on single posts.

## [1.17.3] - 2017-04-19

- Removed propagated media deletions on saving/trashing/hiding.

## [1.17.2] - 2017-04-15

- Avoided showing featured posts on pages beyond the first at the home index.
  This was an unwanted behavior that triggered when removed featured post limits.

## [1.17.1] - 2017-04-14

- Rolled back caching for featured and slider posts.

## [1.17.0] - 2017-04-13

- Improved settings layout.
- Added caching for featured and slider posts.

## [1.16.2] - 2017-04-11

- Added extenders for reporting posts to tickets instead of the contact form
  if Triklet is installed and enabled.

## [1.16.1] - 2017-04-10

- Removed limits for featured posts and the home slider.

## [1.16.0] - 2017-04-08

- Added extension point on post save.
- Added extenders to improve the categories browser.
- Removed title prefix for posts indexes by category.
- Added HTML classes to category index title elements for easing CSS customizations.

## [1.15.4] - 2017-04-05

- Added flags on the posting form to internally identify autosaves and previews.

## [1.15.3] - 2017-04-04

- Added extension points for actions over posts (trashing, change status, etc.)
- Added function hooks for records browser extenders.

## [1.15.2] - 2017-04-03

- Added a workaround for a diff check before saving a post.
- Added Changelog.
- Added extension point for custom author info on the posts browser.

## [1.15.1] - 2017-03-23

- Added support for shortcodes.

## [1.15.0] - 2017-03-22

- Added limitations for categories per user levels on posting forms.

## [1.14.6] - 2017-03-22

- Implemented selector for post permalinks style (slug or post id).
