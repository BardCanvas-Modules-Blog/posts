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
    public $parent_post_title;
    public $parent_post_slug;
    public $children_count;
    
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
    public $featured_media_type;
    
    # Taken with a group_concat from other tables:
    public $tags_list       = array(); # from post_tags
    public $categories_list = array(); # from post_categories
    public $media_list      = array(); # from post_media
    public $mentions_list   = array(); # from post_mentions
    
    private $_author_account;
    private $_meta = array();
    
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
        
        if( $this->featured_media_type != "image" )
        {
            $this->featured_image_thumbnail = "";
            $this->featured_image_path      = "";
        }
        else
        {
            if( ! empty($this->featured_image_thumbnail) )
                $this->featured_image_thumbnail = "{$config->full_root_path}/mediaserver/{$this->featured_image_thumbnail}";
    
            if( ! empty($this->featured_image_path) )
                $this->featured_image_path = "{$config->full_root_path}/mediaserver/{$this->featured_image_path}";
        }
        
        if( is_string($this->tags_list) )       $this->tags_list       = explode(",", $this->tags_list);
        if( is_string($this->categories_list) ) $this->categories_list = explode(",", $this->categories_list);
        if( is_string($this->media_list) )      $this->media_list      = explode(",", $this->media_list);
        if( is_string($this->mentions_list) )   $this->mentions_list   = explode(",", $this->mentions_list);
        
        # Data overrides
        $config->globals["current_post_record"] =& $this;
        $modules["posts"]->load_extensions("post_record_class", "set_from_object");
        unset( $config->globals["current_post_record"] );
        
        # Media tuning
        if( ! preg_match('/.png|.gif|.jpg|.jpeg/i', $this->featured_image_thumbnail) ) $this->featured_image_thumbnail = "";
        if( ! preg_match('/.png|.gif|.jpg|.jpeg/i', $this->featured_image_path) )      $this->featured_image_path = "";
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
            $return["last_viewed"],
            $return["parent_post_title"],
            $return["parent_post_slug"],
            $return["children_count"],
            
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
            $return["featured_media_type"],
            
            $return["tags_list"],
            $return["categories_list"],
            $return["media_list"],
            $return["mentions_list"],
            $return["_meta"]
        );
        
        foreach( $return as $key => &$val )
            if( is_string($val) )
                $val = addslashes($val);
        
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
        global $config, $modules;
        
        $contents = $this->author_display_name;
        $contents = convert_emojis($contents);
        
        if( isset($this->_author_account) ) $config->globals["processing_account"] = $this->_author_account;
        $config->globals["processing_id_account"]  = $this->id_author;
        $config->globals["processing_contents"] = $contents;
        $modules["posts"]->load_extensions("post_record_class", "get_processed_author_display_name");
        $contents = $config->globals["processing_contents"];
        unset( $config->globals["processing_contents"] );
        
        return $contents;
    }
    
    public function get_processed_content()
    {
        global $config, $modules;
        
        $contents = $this->content;
        
        if( stristr($contents, "[post_family_tree") === false )
            if( $this->children_count || $this->parent_post )
                $contents = "[post_family_tree]" . $contents;
        
        $contents = preg_replace(
            '@<p>(https?://([-\w\.]+[-\w])+(:\d+)?(/([\%\w/_\.#-]*(\?\S+)?[^\.\s])?)?)</p>@',
            '<p><a href="$1" target="_blank">$1</a></p>',
            $contents
        );
        
        $contents = convert_shortcodes($contents);
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
        global $config, $settings;
        
        $style = $settings->get("modules:posts.permalink_style");
        switch( $style )
        {
            case "slug": $handler = $this->slug;    break;
            default:     $handler = $this->id_post; break;
        }
        
        if( $fully_qualified ) return "{$config->full_root_url}/{$handler}";
        
        return "{$config->full_root_path}/{$handler}";
    }
    
    /**
     * Returns the permalink to the post
     *
     * @param bool $fully_qualified
     *
     * @return string
     */
    public function get_parent_permalink($fully_qualified = false)
    {
        global $config, $settings;
        
        $style = $settings->get("modules:posts.permalink_style");
        switch( $style )
        {
            case "slug": $handler = $this->parent_post_slug; break;
            default:     $handler = $this->parent_post;      break;
        }
        
        if( $fully_qualified ) return "{$config->full_root_url}/{$handler}";
        
        return "{$config->full_root_path}/{$handler}";
    }
    
    public function can_be_edited()
    {
        global $settings, $account;
        
        if( $account->state != "enabled" ) return false;
        if( $account->level >= config::MODERATOR_USER_LEVEL ) return true;
        if( $this->id_author != $account->id_account ) return false;
        if( $this->publishing_date == "0000-00-00 00:00:00" ) return true;
        if( $this->publishing_date > date("Y-m-d H:i:s") ) return true;
        
        $min_level = (int) $settings->get("modules:posts.level_allowed_to_edit_with_comments");
        if( empty($min_level) ) $min_level = config::NEWCOMER_USER_LEVEL;
        if( $this->comments_count > 0 && $account->level < $min_level ) return false;
        
        $time = (int) $settings->get("modules:posts.time_allowed_for_editing_after_publishing");
        if( empty($time) ) return false;
        
        if( $time > 0 )
        {
            $boundary = strtotime("{$this->publishing_date} + $time minutes");
            if( time() < $boundary ) return true;
        }
        
        if( $settings->get("modules:posts.lock_posts_with_enforced_expiration") == "true"
            && $this->expiration_date != "0000-00-00 00:00:00"
            && $this->expiration_date >= date("Y-m-d H:i:s") ) return true;
        
        if( $time < 0 ) return true;
        
        return false;
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
    
    public function can_be_deleted()
    {
        global $account, $settings;
        
        if( $account->level >= config::MODERATOR_USER_LEVEL ) return true;
        if( $account->level < config::AUTHOR_USER_LEVEL && $this->comments_count > 0 ) return false;
        if( $account->id_account != $this->id_author ) return false;
        if( $this->status == "trashed" ) return false;
        
        if( $settings->get("modules:posts.lock_posts_with_enforced_expiration") == "true"
            && $this->expiration_date != "0000-00-00 00:00:00"
            && $this->expiration_date > date("Y-m-d H:i:s") ) return false;
        
        return true;
    }
    
    private function preload_metas()
    {
        $this->_meta = $this->fetch_all_metas();
    }
    
    /**
     * @param bool $include_hidden
     *
     * @return array
     */
    public function fetch_all_metas($include_hidden = true)
    {
        global $database;
        
        if( empty($this->id_post) ) return array();
        
        $query = $include_hidden 
            ? "select * from post_meta where id_post = '{$this->id_post}'" 
            : "select * from post_meta where id_post = '{$this->id_post}' and name not like '.%'";
        
        $res = $database->query($query);
        if( $database->num_rows($res) == 0 ) return array();
        
        $return = array();
        while($row = $database->fetch_object($res))
            $return[$row->name] = unserialize($row->value);
        
        return $return;
    }
    
    public function get_meta($name)
    {
        if( empty($this->id_post) ) return null;
        
        $this->preload_metas();
        
        return $this->_meta[$name];
    }
    
    public function set_meta($name, $value)
    {
        global $database;
        
        if( empty($this->id_post) ) return;
        
        $this->preload_metas();
        $this->_meta[$name] = $value;
        
        $name  = addslashes($name);
        $value = addslashes(serialize($value));
        $database->exec("
            insert into post_meta (
                id_post,
                name,
                value
            ) values (
                '{$this->id_post}',
                '$name',
                '$value'
            ) on duplicate key update
                value = '$value'
        ");
    }
    
    public function purge_metas()
    {
        global $database;
        
        $database->exec("delete from post_meta where id_post = '{$this->id_post}'");
        $this->_meta = array();
    }
}
