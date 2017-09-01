
# Posts Module Change Log

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
