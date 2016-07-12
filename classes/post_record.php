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
    public $main_category     ; # varchar(32) not null default '',
    
    public $visibility        ; # enum('public', 'private', 'users', 'friends', 'level_based') not null default 'public',
    public $status            ; # enum('draft', 'published', 'reviewing', 'hidden', 'trashed') not null default 'draft',
    public $password          ; # varchar(32) not null default '',
    public $allow_comments    ; # tinyint unsigned not null default 1,
    
    public $creation_date     ; # datetime,
    public $creation_ip       ; # varchar(15) not null default '',
    public $creation_host     ; # varchar(255) not null default '',
    public $creation_location ; # varchar(255) not null default '',
    
    public $publishing_date   ; # date,
    public $views             ; # int unsigned not null default 0,
    public $comments_count    ; # int unsigned not null default 0,
    
    public $last_update       ; # datetime,
    public $last_viewed       ; # datetime,
    public $last_commented    ; # datetime,
    
    public $id_featured_image ; # varchar(32) not null default '',
    
    # Dynamically added:
    public $author_user_name;
    public $author_display_name;
    public $author_email;
    public $author_level;
    
    # Taken with a group_concat from other tables:
    public $tags_list         ; # from post_tags
    public $categories_list   ; # from post_categories
    public $media_list        ; # from post_media
    public $mentions_list     ; # from post_mentions
    
    protected function set_from_object($object_or_array)
    {
        parent::set_from_object($object_or_array);
        
        if( ! empty($this->_author_data) )
        {
            $parts = explode("\t", $this->_author_data);
    
            $this->author_user_name    = $parts[0];
            $this->author_display_name = $parts[1];
            $this->author_email        = $parts[2];
            $this->author_level        = $parts[3];
            
            unset($this->_author_data);
        }
    }
    
    public function set_new_id()
    {
        $this->id_post = uniqid();
    }
}
