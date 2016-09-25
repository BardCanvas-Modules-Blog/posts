<?php
namespace hng2_modules\posts;

use hng2_base\account;
use hng2_base\account_record;
use hng2_base\accounts_repository;
use hng2_base\config;
use hng2_base\module;
use hng2_repository\abstract_record;

class post_record extends abstract_record
{
    public $id_post           ; # bigint unsigned not null default 0,
    public $parent_post       ; # bigint unsigned not null default 0,
    public $id_author         ; # bigint unsigned not null default 0,
    
    public $slug              ; # varchar(128) not null default '',
    public $title             ; # varchar(255) not null default '',
    public $excerpt           ; # varchar(255) not null default '',
    public $content           ; # longtext,
    public $main_category     ; # varchar(32) not null default '',
    
    public $visibility        ; # enum('public', 'private', 'users', 'friends', 'level_based') not null default 'public',
    public $status            ; # enum('draft', 'published', 'reviewing', 'hidden', 'trashed') not null default 'draft',
    public $password          ; # varchar(32) not null default '',
    public $allow_comments    ; # tinyint unsigned not null default 1,

    public $pin_to_home                ; # tinyint unsigned not null default 0,
    public $pin_to_main_category_index ; # tinyint unsigned not null default 0,
    
    public $creation_date     ; # datetime,
    public $creation_ip       ; # varchar(15) not null default '',
    public $creation_host     ; # varchar(255) not null default '',
    public $creation_location ; # varchar(255) not null default '',
    
    public $publishing_date   ; # datetime,
    public $expiration_date   ; # datetime,
    public $comments_count    ; # int unsigned not null default 0,
    
    public $last_update       ; # datetime,
    public $last_commented    ; # datetime,
    
    public $id_featured_image ; # bigint unsigned not null default 0,
    
    # TODO:                                                                                                        :
    # TODO:  IMPORTANT! All dinamically generated members below should be undefined in get_for_database_insertion! :
    # TODO:                                                                                                        :
    
    # Dynamically added:
    
    public $views;
    public $last_viewed;
    
    public $author_user_name;
    public $author_display_name;
    public $author_email;
    public $author_level;
    
    public $main_category_slug;
    public $main_category_title;
    public $main_category_visibility;
    public $main_category_min_level;
    
    public $featured_image_thumbnail;
    public $featured_image_path;
    
    # Taken with a group_concat from other tables:
    public $tags_list       = array(); # from post_tags
    public $categories_list = array(); # from post_categories
    public $media_list      = array(); # from post_media
    public $mentions_list   = array(); # from post_mentions
    
    private $_author_account;
    
    protected function set_from_object($object_or_array)
    {
        /** @var module[] $modules */
        global $config, $modules;
        
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
        
        if( ! empty($this->_main_category_data) )
        {
            $parts = explode("\t", $this->_main_category_data);
    
            $this->main_category_slug       = $parts[0];
            $this->main_category_title      = $parts[1];
            $this->main_category_visibility = $parts[2];
            $this->main_category_min_level  = $parts[3];
            
            unset($this->_main_category_data);
        }
        
        if( ! empty($this->featured_image_thumbnail) )
            $this->featured_image_thumbnail = "{$config->full_root_path}/mediaserver/{$this->featured_image_thumbnail}";
        
        if( ! empty($this->featured_image_path) )
            $this->featured_image_path = "{$config->full_root_path}/mediaserver/{$this->featured_image_path}";
        
        if( is_string($this->tags_list) )       $this->tags_list       = explode(",", $this->tags_list);
        if( is_string($this->categories_list) ) $this->categories_list = explode(",", $this->categories_list);
        if( is_string($this->media_list) )      $this->media_list      = explode(",", $this->media_list);
        if( is_string($this->mentions_list) )   $this->mentions_list   = explode(",", $this->mentions_list);
        
        # Data overrides
        $config->globals["current_post_record"] =& $this;
        $modules["posts"]->load_extensions("post_record_class", "set_from_object");
        unset( $config->globals["current_post_record"] );
    }
    
    public function set_from_post()
    {
        parent::set_from_post();
        
        $this->content = str_replace("<p> </p>",      "<p></p>", $this->content);
        $this->content = str_replace("<p>&nbsp;</p>", "<p></p>", $this->content);
        
        if( empty($this->visibility) ) $this->visibility = "public";
    }
    
    public function set_new_id()
    {
        $this->id_post = make_unique_id("50");
    }
    
    /**
     * @return object
     */
    public function get_for_database_insertion()
    {
        $return = (array) $this;
        
        unset(
            $return["views"],
            $return["last_view"],
            
            $return["author_user_name"],
            $return["author_display_name"],
            $return["author_email"],
            $return["author_level"],
            
            $return["main_category_slug"],
            $return["main_category_title"],
            $return["main_category_visibility"],
            $return["main_category_min_level"],
            
            $return["featured_image_thumbnail"],
            $return["featured_image_path"],
            
            $return["tags_list"],
            $return["categories_list"],
            $return["media_list"],
            $return["mentions_list"]
        );
        
        foreach( $return as $key => &$val ) $val = addslashes($val);
        
        return (object) $return;
    }
    
    /**
     * @param null|account|account_record $prefetched_author_record
     */
    public function set_author($prefetched_author_record)
    {
        $this->_author_account = $prefetched_author_record;
    }
    
    /**
     * @return account
     */
    public function get_author()
    {
        // TODO: Implement accounts repository for caching
        
        if( is_object($this->_author_account) ) return $this->_author_account;
    
        $repository = new accounts_repository();
        return $repository->get($this->id_author);
    }
    
    /**
     * Returns the title with all output processing.
     *
     * @param bool $include_autolinks If false, <a> tags wont be added. Useful when the title is inserted into an <a> tag.
     *
     * @return string
     */
    public function get_processed_title($include_autolinks = true)
    {
        global $config;
        
        $contents = $this->title;
        $contents = convert_emojis($contents);
        if( $include_autolinks ) $contents = autolink_hash_tags($contents, "{$config->full_root_path}/tag/");
        
        # TODO: Add get_processed_title() extension point
        
        return $contents;
    }
    
    /**
     * Returns the excerpt with all output processing.
     *
     * @param bool $strip_block_tags Strips block-level tags, i.e. <div>, <blockquote>, etc.
     * @return string
     */
    public function get_processed_excerpt($strip_block_tags = false)
    {
        global $config, $modules;
        
        $contents = $this->excerpt;
        $contents = convert_emojis($contents);
        $contents = autolink_hash_tags($contents, "{$config->full_root_path}/tag/");
        
        $config->globals["processing_contents"] = $contents;
        $modules["posts"]->load_extensions("post_record_class", "get_processed_excerpt");
        $contents = $config->globals["processing_contents"];
        
        if( $strip_block_tags ) $contents = strip_tags($contents, "<a><b><i><span><strong><em>");
        
        return $contents;
    }
    
    /**
     * Returns the display name with all output processing
     */
    public function get_processed_author_display_name()
    {
        global $config;
        
        $contents = $this->author_display_name;
        $contents = convert_emojis($contents);
        $contents = autolink_hash_tags($contents, "{$config->full_root_path}/tag/");
        
        # TODO: Add get_processed_author_display_name() extension point
        
        return $contents;
    }
    
    public function get_processed_content()
    {
        global $config, $modules;
        
        $contents = $this->content;
        $contents = convert_emojis($contents);
        
        $contents = convert_media_tags($contents);
        $contents = autolink_hash_tags($contents, "{$config->full_root_path}/tag/");
        
        $config->globals["processing_contents"] = $contents;
        $modules["posts"]->load_extensions("post_record_class", "get_processed_content");
        $contents = $config->globals["processing_contents"];
        
        return $contents;
    }
    
    /**
     * Returns the permalink to the post
     *
     * @param bool $fully_qualified
     *
     * @return string
     */
    public function get_permalink($fully_qualified = false)
    {
        global $config;
        
        if( $fully_qualified ) return "{$config->full_root_url}/{$this->id_post}";
        
        return "{$config->full_root_path}/{$this->id_post}";
    }
    
    public function can_be_edited()
    {
        global $settings, $account;
        
        if( ! $account->_exists ) return false;
        if( $account->level >= config::MODERATOR_USER_LEVEL ) return true;
        if( $this->publishing_date == "0000-00-00 00:00:00" ) return true;
        if( $this->comments_count > 0 ) return false;
        
        $time = (int) $settings->get("modules:posts.time_allowed_for_editing_after_publishing");
        if( empty($time) ) return false;
        
        $boundary = strtotime("{$this->publishing_date} + $time minutes");
        if( time() < $boundary ) return true;
        else                     return false;
    }
    
    public function get_filtered_tags_list()
    {
        global $settings;
        
        $list = $this->tags_list;
        if( empty($list) ) return array();
        
        if( is_string($list) ) $list = explode(",", $list);
        
        $featureds_tag = $settings->get("modules:posts.featured_posts_tag");
        if( empty($featureds_tag) ) return $list;
        if( $settings->get("modules:posts.show_featured_posts_tag_everywhere") == "true" ) return $list;
        
        $key = array_search($featureds_tag, $list);
        if( $key === false ) return $list;
        
        unset($list[$key]);
        return $list;
    }
}
