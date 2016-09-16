<?php
/**
 * Gallery extender - principal data addons
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Globals:
 * @var module       $current_module accounts
 * @var module       $this_module    self
 * @var media_record $row
 */

use hng2_base\config;
use hng2_base\module;
use hng2_media\media_record;
use hng2_modules\posts\post_record;
use hng2_modules\posts\posts_repository;

global $row, $principal_data_addons, $modules, $account;

if( $account->level < config::MODERATOR_USER_LEVEL ) return;

$repository = new posts_repository();

$count = $repository->get_record_count(array(
    "id_post in (select id_post from post_media where id_media = '$row->id_media')",
));
if( $count == 0 ) return;

$records = $repository->find(
    array("id_post in (select id_post from post_media where id_media = '$row->id_media')"),
    0, 0, ""
);

/** @var post_record $record */
foreach($records as $record)
    $principal_data_addons[] = replace_escaped_vars(
        $this_module->language->gallery_addons->principal_data,
        array('{$author}', '{$url}', '{$title}', '{$time}', '{$status}'),
        array(
            convert_emojis($record->author_display_name),
            $record->get_permalink(),
            convert_emojis($record->title),
            $record->publishing_date == "0000-00-00 00:00:00" ? "&mdash;" : get_minimized_date($record->publishing_date),
            $this_module->language->status_labels->{$record->status},
        )
    );
