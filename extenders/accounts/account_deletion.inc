<?php
/**
 * Accounts extender - deletion
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Globals:
 * @var module $current_module accounts
 * @var module $this_module    self
 * @var object $row
 */


use hng2_base\module;

global $database, $modules, $account;

$id_account = $config->globals["deleting_id_account"];
if( empty($id_account) ) return;

if( empty($config->globals["deletions_log"]) ) $config->globals["deletions_log"] = array();
$config->globals["deletions_log"][] = "<b>Starting Posts extender for account deletion</b>";

#
# Delete post categories
#

$count = $database->exec("
    delete from post_categories where id_post in (
      select id_post from posts where id_author = '$id_account'
    )
");
if( $count > 0 ) $config->globals["deletions_log"][] = "$count records from <b>post_categories</b> table deleted.";

#
# Delete post mentions (where user is mentioned)
#

$count = $database->exec("delete from post_mentions where id_account = '$id_account'");
if( $count > 0 ) $config->globals["deletions_log"][] = "$count mentions of the user from <b>post_mentions</b> table deleted.";

#
# Delete post mentions (from user posts)
#

$count = $database->exec("
    delete from post_mentions where id_post in (
      select id_post from posts where id_author = '$id_account'
    )
");
if( $count > 0 ) $config->globals["deletions_log"][] = "$count records owned by the user in <b>post_mentions</b> table deleted.";

#
# Delete post media attachments
#

$count = $database->exec("
    delete from post_media where id_post in (
      select id_post from posts where id_author = '$id_account'
    )
");
if( $count > 0 ) $config->globals["deletions_log"][] = "$count <b>post media attachment</b> relationships deleted.<br><b>Note:</b> media records and files not deleted at this time.";

#
# Delete post tags
#

$count = $database->exec("
    delete from post_tags where id_post in (
      select id_post from posts where id_author = '$id_account'
    )
");
if( $count > 0 ) $config->globals["deletions_log"][] = "$count records from <b>post_tags</b> table deleted.";

#
# Post views
#

$count = $database->exec("
    delete from post_views where id_post in (
      select id_post from posts where id_author = '$id_account'
    )
");
if( $count > 0 ) $config->globals["deletions_log"][] = "$count records from <b>post views</b> table deleted.";

#
# Dependants invocation
#

$modules["posts"]->load_extensions("extenders", "account_deletion_pre_purge");

#
# Post deletions
#

$count = $database->exec("delete from posts where id_author = '$id_account'");
if( $count > 0 ) $config->globals["deletions_log"][] = "$count <b>posts</b> deleted.";

$config->globals["deletions_log"][] = "<b>Finished Posts extender for account deletion</b>";
if( $config->globals["notify_deletion_progress"] )
{
    send_notification(
        $account->id_account, "information", "Finished Posts extender for account deletion"
    );
    sleep(1);
}
