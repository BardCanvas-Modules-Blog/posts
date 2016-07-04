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

if( $module_install_action == "install" )
{
    $query = "
        create table if not exists posts (
            
            id_post           bigint unsigned not null auto_increment,
            parent_post       bigint unsigned not null default 0,
            id_author         varchar(32) not null default '',
            
            slug              varchar(128) not null default '',
            title             varchar(255) not null default '',
            excerpt           varchar(255) not null default '',
            content           longtext,
            
            visibility        enum('public', 'private', 'users', 'friends', 'level_based') not null default 'public',
            status            enum('draft', 'published', 'reviewing', 'hidden', 'trashed') not null default 'draft',
            password          varchar(32) not null default '',
            
            creation_date     datetime,
            creation_host     varchar(255) not null default '',
            creation_location varchar(255) not null default '',
            creation_details  varchar(255) not null default '',
            
            publishing_date   datetime,
            last_update       datetime,
            
            last_viewed       datetime,
            views             datetime,
            
            allow_comments    tinyint unsigned not null default 1,
            last_comment_date datetime,
            
            primary key        ( id_post ),
            index   posts_tree ( id_post, parent_post ),
            index   by_slug    ( slug(5) ),
            index   by_author  ( id_author )
            
        ) engine=InnoDB default charset=utf8mb4 collate='utf8mb4_unicode_ci'
    ";
    
    try
    {
        $database->exec($query);
        $messages[] = replace_escaped_vars($language->install_messages->table_created_ok, '{$table_name}', 'posts');
    }
    catch( \Exception $e )
    {
        $errors[] = replace_escaped_vars(
            $language->install_messages->table_created_ko,
            array( '{$table_name}', '{$error}' ),
            array( 'posts',           $e->getMessage()  )
        );
    }
    
    return;
}

if( $module_install_action == "uninstall" )
{
    $query = "DROP TABLE if exists posts";
    
    try
    {
        $database->exec($query);
        $messages[] = replace_escaped_vars($language->install_messages->table_deleted_ok, '{$table_name}', 'posts');
    }
    catch( \Exception $e )
    {
        $errors[] = replace_escaped_vars(
            $language->install_messages->table_deleted_ko,
            array( '{$table_name}', '{$error}' ),
            array( 'posts',           $e->getMessage()  )
        );
    }
    
    return;
}
