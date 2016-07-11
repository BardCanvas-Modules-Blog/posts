<?php
namespace hng2_modules\posts;

use hng2_base\repository\abstract_record;

class post_record extends abstract_record
{
    public $id_post           ; # bigint unsigned not null default 0,
    public $parent_post       ; # bigint unsigned not null default 0,
    public $id_author         ; # varchar(32) not null default '',
    public $slug              ; # varchar(128) not null default '',
    public $title             ; # varchar(255) not null default '',
    public $excerpt           ; # varchar(255) not null default '',
    public $content           ; # longtext,
    public $tags_cache        ; # varchar(255) not null default '',
    public $categories_cache  ; # varchar(255) not null default '',
    public $media_cache       ; # varchar(255) not null default '',
    public $mentions_cache    ; # varchar(255) not null default '',
    public $visibility        ; # enum('public', 'private', 'users', 'friends', 'level_based') not null default 'public',
    public $status            ; # enum('draft', 'published', 'reviewing', 'hidden', 'trashed') not null default 'draft',
    public $password          ; # varchar(32) not null default '',
    public $creation_date     ; # datetime,
    public $creation_host     ; # varchar(255) not null default '',
    public $creation_location ; # varchar(255) not null default '',
    public $creation_details  ; # varchar(255) not null default '',
    public $publishing_date   ; # datetime,
    public $views             ; # int unsigned not null default 0,
    public $allow_comments    ; # tinyint unsigned not null default 1,
    public $comments_count    ; # int unsigned not null default 0,
    public $last_update       ; # datetime,
    public $last_viewed       ; # datetime,
    public $last_commented    ; # datetime,
    public $id_featured_image ; # bigint unsigned not null default 0,
    
    public function set_new_id()
    {
        $this->id_post = uniqid();
    }
}
