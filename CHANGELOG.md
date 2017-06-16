
# Posts Module Change Log

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
