<?php
/**
 * Module installation file
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * Trailing vars:
 * @var string $module_install_action install, uninstall, enable, disable
 * @var array  $messages
 * @var array  $errors
 * @var module $this_module
 * @var module $current_module
 */

use hng2_base\module;

$tables = array(
    "posts" => "
        create table if not exists posts (
            
            id_post           bigint unsigned not null auto_increment,
            parent_post       bigint unsigned not null default 0,
            id_author         varchar(32) not null default '',
            
            slug              varchar(128) not null default '',
            title             varchar(255) not null default '',
            excerpt           varchar(255) not null default '',
            content           longtext,
            
            tags_cache        varchar(255) not null default '',
            categories_cache  varchar(255) not null default '',
            media_cache       varchar(255) not null default '',
            mentions_cache    varchar(255) not null default '',
            
            visibility        enum('public', 'private', 'users', 'friends', 'level_based') not null default 'public',
            status            enum('draft', 'published', 'reviewing', 'hidden', 'trashed') not null default 'draft',
            password          varchar(32) not null default '',
            
            creation_date     datetime,
            creation_host     varchar(255) not null default '',
            creation_location varchar(255) not null default '',
            creation_details  varchar(255) not null default '',
            
            publishing_date   datetime,
            views             int unsigned not null default 0,
            allow_comments    tinyint unsigned not null default 1,
            comments_count    int unsigned not null default 0,
            
            last_update       datetime,
            last_viewed       datetime,
            last_commented    datetime,
            
            id_featured_image bigint unsigned not null default 0,
            
            primary key        ( id_post ),
            index   posts_tree ( id_post, parent_post ),
            index   by_slug    ( slug(5) ),
            index   by_author  ( id_author )
            
        ) engine=InnoDB default charset=utf8mb4 collate='utf8mb4_unicode_ci'
    ",
    "post_media" => "
        create table if not exists post_media (
            
            id_post       bigint unsigned not null default 0,
            id_media      bigint unsigned not null default 0,
            date_attached datetime,
            
            primary key ( id_post, id_media )
            
        ) engine=InnoDB default charset=utf8mb4 collate='utf8mb4_unicode_ci'
    ",
    "post_tags" => "
        create table if not exists post_tags (
            
            id_post       bigint unsigned not null default 0,
            tag           varchar(100) not null default '',
            date_attached datetime,
            
            primary key ( id_post, tag )
            
        ) engine=InnoDB default charset=utf8mb4 collate='utf8mb4_unicode_ci'
    ",
    "post_categories" => "
        create table if not exists post_categories (
            
            id_post       bigint unsigned not null default 0,
            id_category   varchar(32) not null default '',
            date_attached datetime,
            
            primary key ( id_post, id_category )
            
        ) engine=InnoDB default charset=utf8mb4 collate='utf8mb4_unicode_ci'
    ",
    "post_mentions" => "
        create table if not exists post_mentions (
            
            id_post       bigint unsigned not null default 0,
            id_account    varchar(32) not null default '',
            date_attached datetime,
            
            primary key ( id_post, id_account )
            
        ) engine=InnoDB default charset=utf8mb4 collate='utf8mb4_unicode_ci'
    ",
);

if( $module_install_action == "install" )
{
    foreach($tables as $table_name => $query)
    {
        try
        {
            $database->exec($query);
            $messages[] = replace_escaped_vars($language->install_messages->table_created_ok, '{$table_name}', $table_name);
        }
        catch( \Exception $e )
        {
            $errors[] = replace_escaped_vars(
                $language->install_messages->table_created_ko,
                array( '{$table_name}', '{$error}' ),
                array(   $table_name,     $e->getMessage()  )
            );
        }
    }
    
    return;
}

if( $module_install_action == "uninstall" )
{
    foreach($tables as $table_name => $query)
    {
        try
        {
            $database->exec("DROP TABLE if exists {$table_name}");
            $messages[] = replace_escaped_vars($language->install_messages->table_deleted_ok, '{$table_name}', $table_name);
        }
        catch( \Exception $e )
        {
            $errors[] = replace_escaped_vars(
                $language->install_messages->table_deleted_ko,
                array( '{$table_name}', '{$error}' ),
                array(   $table_name,     $e->getMessage() )
            );
        }
    }
    
    return;
}
