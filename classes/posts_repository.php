<?php
namespace hng2_modules\posts;

use hng2_repository\abstract_repository;
use hng2_base\accounts_repository;
use hng2_modules\categories\category_record;
use hng2_tools\record_browser;

class posts_repository extends abstract_repository
{
    protected $row_class                = "hng2_modules\\posts\\post_record";
    protected $table_name               = "posts";
    protected $key_column_name          = "id_post";
    protected $additional_select_fields = array();
    
    public function __construct()
    {
        global $settings;
        
        parent::__construct();
        
        $this->additional_select_fields[] = "
        ( select concat(user_name, '\\t', display_name, '\\t', email, '\\t', level)
           from account where account.id_account = posts.id_author
           ) as _author_data";
        $this->additional_select_fields[] = "
        ( select concat(slug, '\\t', title)
           from categories where categories.id_category = posts.main_category
           ) as _main_category_data";
        
        if( $settings->get("modules:posts.automatic_featured_images") != "true" )
        {
            $this->additional_select_fields[] = "
            ( select thumbnail
               from media where media.id_media = posts.id_featured_image
               ) as featured_image_thumbnail";
        }
        else
        {
            $this->additional_select_fields[] = "
            if(
              posts.id_featured_image <> '', 
              ( select path from media where media.id_media = posts.id_featured_image ),
              ( select path from media
                join post_media on post_media.id_media = media.id_media
                where post_media.id_post = posts.id_post
                order by date_attached asc, order_attached asc limit 1 )
            ) as featured_image_path";
            $this->additional_select_fields[] = "
            if(
              posts.id_featured_image <> '', 
              ( select thumbnail from media where media.id_media = posts.id_featured_image ),
              ( select thumbnail from media
                join post_media on post_media.id_media = media.id_media
                where post_media.id_post = posts.id_post
                order by date_attached asc, order_attached asc limit 1 )
            ) as featured_image_thumbnail";
        }
        
        $this->additional_select_fields[] = "
        ( select group_concat(tag order by date_attached asc, order_attached asc separator ',')
           from post_tags where post_tags.id_post = posts.id_post
           ) as tags_list";
        $this->additional_select_fields[] = "
        ( select group_concat(id_media order by date_attached asc, order_attached asc separator ',')
           from post_media where post_media.id_post = posts.id_post
           ) as media_list";
        $this->additional_select_fields[] = "
        ( select group_concat(id_category order by date_attached asc, order_attached asc separator ',')
           from post_categories where post_categories.id_post = posts.id_post
           ) as categories_list";
        $this->additional_select_fields[] = "
        ( select group_concat(id_account order by date_attached asc, order_attached asc separator ',')
           from post_mentions where post_mentions.id_post = posts.id_post
           ) as mentions_list";
    }
    
    /**
     * @param $id
     *
     * @return post_record|null
     */
    public function get($id)
    {
        return parent::get($id);
    }
    
    /**
     * @param array $ids
     *
     * @return post_record[]
     */
    public function get_multiple(array $ids)
    {
        if( count($ids) == 0 ) return array();
        
        $prepared_ids = array();
        foreach($ids as $id) $prepared_ids[] = "'$id'";
        $prepared_ids = implode(", ", $prepared_ids);
        
        $res = $this->find(array("id_post in ($prepared_ids)"), 0, 0, "");
        if( count($res) == 0 ) return array();
        
        $return = array();
        foreach($res as $post) $return[$post->id_post] = $post;
        
        return $return;
    }
    
    /**
     * @param array  $where
     * @param int    $limit
     * @param int    $offset
     * @param string $order
     *
     * @return post_record[]
     */
    public function find($where, $limit, $offset, $order)
    {
        return parent::find($where, $limit, $offset, $order);
    }
    
    /**
     * @param $id_or_slug
     *
     * @return post_record|null
     */
    public function get_by_id_or_slug($id_or_slug)
    {
        $res = $this->find(array("id_post = '$id_or_slug' or slug = '$id_or_slug'"), 0, 0, "");
        
        if( empty($res) ) return null;
        
        return current($res);
    }
    
    /**
     * @param post_record $record
     *
     * @return int
     */
    public function save($record)
    {
        global $database;
        
        $this->validate_record($record);
        $obj = $record->get_for_database_insertion();
        
        $obj->last_update = date("Y-m-d H:i:s");
        
        $res =  $database->exec("
            insert into {$this->table_name}
            (
                id_post          ,
                parent_post      ,
                id_author        ,
                
                slug             ,
                title            ,
                excerpt          ,
                content          ,
                main_category    ,
                
                visibility       ,
                status           ,
                password         ,
                allow_comments   ,
                
                pin_to_home                ,
                pin_to_main_category_index ,
                
                creation_date    ,
                creation_ip      ,
                creation_host    ,
                creation_location,
                
                publishing_date  ,
                expiration_date  ,
                last_update      ,
                id_featured_image
            ) values (
                '{$obj->id_post          }',
                '{$obj->parent_post      }',
                '{$obj->id_author        }',
                
                '{$obj->slug             }',
                '{$obj->title            }',
                '{$obj->excerpt          }',
                '{$obj->content          }',
                '{$obj->main_category    }',
                
                '{$obj->visibility       }',
                '{$obj->status           }',
                '{$obj->password         }',
                '{$obj->allow_comments   }',
                
                '{$obj->pin_to_home               }',
                '{$obj->pin_to_main_category_index}',
                
                '{$obj->creation_date    }',
                '{$obj->creation_ip      }',
                '{$obj->creation_host    }',
                '{$obj->creation_location}',
                
                '{$obj->publishing_date  }',
                '{$obj->expiration_date  }',
                '{$obj->last_update      }',
                '{$obj->id_featured_image}'
            ) on duplicate key update
                parent_post       = '{$obj->parent_post      }',
                
                slug              = '{$obj->slug             }',
                title             = '{$obj->title            }',
                excerpt           = '{$obj->excerpt          }',
                content           = '{$obj->content          }',
                main_category     = '{$obj->main_category    }',
                
                visibility        = '{$obj->visibility       }',
                status            = '{$obj->status           }',
                password          = '{$obj->password         }',
                allow_comments    = '{$obj->allow_comments   }',
                
                pin_to_home                = '{$obj->pin_to_home               }', 
                pin_to_main_category_index = '{$obj->pin_to_main_category_index}',
                
                expiration_date   = '{$obj->expiration_date  }',
                last_update       = '{$obj->last_update      }',
                id_featured_image = '{$obj->id_featured_image}'
        ");
        $this->last_query = $database->get_last_query();
        
        return $res;
    }
    
    /**
     * @param post_record $record
     *
     * @throws \Exception
     */
    public function validate_record($record)
    {
        if( ! $record instanceof post_record )
            throw new \Exception(
                "Invalid object class! Expected: {$this->row_class}, received: " . get_class($record)
            );
    }
    
    public function set_category($id_category, $id_post)
    {
        global $database;
        
        $attached = $this->get_attached_categories($id_post);
        if( isset($attached[$id_category]) ) return 0;
        
        $order = microtime(true);
        $date  = date("Y-m-d H:i:s");
        $res = $database->exec("
            insert into post_categories set
            id_post        = '$id_post',
            id_category    = '$id_category',
            date_attached  = '$date',
            order_attached = '$order'
        ");
        $this->last_query = $database->get_last_query();
        
        return $res;
    }
    
    /**
     * @param $id_post
     *
     * @return post_tag[]
     * 
     * @throws \Exception
     */
    public function get_tags($id_post)
    {
        global $database;
        
        $res = $database->query("select * from post_tags where id_post = '$id_post'");
        $this->last_query = $database->get_last_query();
        
        if( $database->num_rows($res) == 0 ) return array();
        
        $rows = array();
        while($row = $database->fetch_object($res))
            $rows[$row->tag] = new post_tag($row);
        
        return $rows;
    }
    
    public function set_tags(array $list, $id_post)
    {
        global $database;
        
        $actual_tags = $this->get_tags($id_post);
        
        if( empty($actual_tags) && empty($list) ) return;
        
        $date = date("Y-m-d H:i:s");
        $inserts = array();
        $index   = 1;
        foreach($list as $tag)
        {
            if( ! isset($actual_tags[$tag]) ) $inserts[] = "('$id_post', '$tag', '$date', '$index')";
            unset($actual_tags[$tag]);
            $index++;
        }
        
        if( ! empty($inserts) )
        {
            $database->exec(
                "insert into post_tags (id_post, tag, date_attached, order_attached) values "
                . implode(", ", $inserts)
            );
            $this->last_query = $database->get_last_query();
        }
        
        if( ! empty($actual_tags) )
        {
            $deletes = array();
            foreach($actual_tags as $tag => $object) $deletes[] = "'$tag'";
            $database->exec(
                "delete from post_tags where id_post = '$id_post' and tag in (" . implode(", ", $deletes) . ")"
            );
            $this->last_query = $database->get_last_query();
        }
    }
    
    /**
     * @param $id_post
     *
     * @return post_media_item[]
     *
     * @throws \Exception
     */
    public function get_media_items($id_post)
    {
        global $database;
        
        $res = $database->query("select * from post_media where id_post = '$id_post' order by date_attached, order_attached");
        $this->last_query = $database->get_last_query();
        
        if( $database->num_rows($res) == 0 ) return array();
        
        $rows = array();
        while($row = $database->fetch_object($res))
            $rows[$row->id_media] = new post_media_item($row);
        
        return $rows;
    }
    
    public function set_media_items(array $list, $id_post)
    {
        global $database;
        
        $actual_items = $this->get_media_items($id_post);
        
        if( empty($actual_items) && empty($list) ) return;
        
        $date    = date("Y-m-d H:i:s");
        $inserts = array();
        $index   = 1;
        foreach($list as $id)
        {
            if( ! isset($actual_items[$id]) ) $inserts[] = "('$id_post', '$id', '$date', '$index')";
            unset($actual_items[$id]);
            $index++;
        }
        
        if( ! empty($inserts) )
        {
            $database->exec(
                "insert into post_media (id_post, id_media, date_attached, order_attached) values "
                . implode(", ", $inserts)
            );
            $this->last_query = $database->get_last_query();
        }
        
        if( ! empty($actual_items) )
        {
            $deletes = array();
            foreach($actual_items as $id => $object) $deletes[] = "'$id'";
            $database->exec(
                "delete from post_media where id_post = '$id_post' and id_media in (" . implode(", ", $deletes) . ")"
            );
            $this->last_query = $database->get_last_query();
        }
    }
    
    public function unset_category($id_category, $id_post)
    {
        global $database;
        
        $res = $database->exec("
            delete from post_categories where
            id_post     = '$id_post' and
            id_category = '$id_category'
        ");
        $this->last_query = $database->get_last_query();
        
        return $res;
    }
    
    /**
     * @param $id_post
     *
     * @return category_record[]
     * 
     * @throws \Exception
     */
    public function get_attached_categories($id_post)
    {
        global $database;
        
        $res = $database->query("
            select
                post_categories.order_attached,
                categories.*
            from
                post_categories, categories
            where
                post_categories.id_post = '$id_post' and
                categories.id_category  = post_categories.id_category
            order by
                post_categories.date_attached, post_categories.order_attached
        ");
        $this->last_query = $database->get_last_query();
        
        if( $database->num_rows($res) == 0 ) return array();
        
        $return = array();
        while($row = $database->fetch_object($res))
            $return[$row->id_category] = new category_record($row);
        
        return $return;
    }
    
    public function trash($id_post)
    {
        global $database;
        
        # TODO: Hide all attached media?
        
        $date = date("Y-m-d H:i:s");
        
        $res = $database->exec("
            update posts set
                status      = 'trashed',
                last_update = '$date'
            where
                id_post = '$id_post'
        ");
        $this->last_query = $database->get_last_query();
        
        return $res;
    }
    
    /**
     * Posts index builder
     * Used to build indexes by user/category/tag/date
     *
     * @param array $where Initial params
     *
     * @param bool  $skip_date_checks
     *
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params($where = array(), $skip_date_checks = false)
    {
        global $settings;
        
        $today = date("Y-m-d H:i:s");
        $where[] = "status = 'published'";
        $where[] = "visibility <> 'private'";
        
        if( ! $skip_date_checks )
        {
            $where[] = "publishing_date <= '$today'";
            $where[] = "(expiration_date = '0000-00-00 00:00:00' or expiration_date > '$today' )";
        }
        
        $order  = "publishing_date desc";
        $limit  = $settings->get("modules:posts.items_per_page", 30);
        $offset = (int) $_GET["offset"];
        
        if( empty($limit) ) $limit = 30;
        
        return (object) array(
            "where"  => $where,
            "limit"  => $limit,
            "offset" => $offset,
            "order"  => $order
        );
    }
    
    /**
     * Returns find params for home EXCLUDING featured posts
     * 
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params_for_home()
    {
        global $settings;
        
        $return = $this->build_find_params();
        
        $ttl = $settings->get("modules:posts.featured_posts_ttl"); if( empty($ttl) ) $ttl = 0;
        $now = date("Y-m-d H:i:s");
        
        if( $ttl > 0 )
            $return->where[]
                = "id_post not in
                   (
                       select post_tags.id_post from post_tags
                       where post_tags.id_post = posts.id_post
                       and post_tags.tag       = '{$settings->get("modules:posts.featured_posts_tag")}'
                       and date_add(post_tags.date_attached, interval $ttl hour) > '$now' 
                   )";
        else
            $return->where[]
                = "id_post not in
                   (
                       select post_tags.id_post from post_tags
                       where post_tags.id_post = posts.id_post
                       and   post_tags.tag     = '{$settings->get("modules:posts.featured_posts_tag")}'
                   )";
        
        return $return;
    }
    
    /**
     * Returns find params for home INCLUDING ONLY featured posts
     * 
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params_for_featured_posts()
    {
        global $settings;
        
        $return = $this->build_find_params();
        
        $ttl = $settings->get("modules:posts.featured_posts_ttl"); if( empty($ttl) ) $ttl = 0;
        $now = date("Y-m-d H:i:s");
        
        if( $ttl > 0 )
            $return->where[]
                = "id_post in
                   (
                     select post_tags.id_post from post_tags
                     where post_tags.id_post = posts.id_post
                     and   post_tags.tag     = '{$settings->get("modules:posts.featured_posts_tag")}'
                     and   date_add(post_tags.date_attached, interval $ttl hour) > '$now'
                   )";
        else
            $return->where[]
                = "id_post in
                   (
                     select post_tags.id_post from post_tags
                     where post_tags.id_post = posts.id_post
                     and   post_tags.tag     = '{$settings->get("modules:posts.featured_posts_tag")}'
                   )";
        
        return $return;
    }
    
    
    /**
     * Returns find params for home INCLUDING ONLY posts in slider categories
     * 
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params_for_posts_slider()
    {
        global $settings;
        
        $return = $this->build_find_params();
        
        $slider_categories = trim($settings->get("modules:posts.slider_categories"));
        
        if( empty($slider_categories) )
        {
            $return->where[] = "id_post = '_NONE_'";
            
            return $return;
        }
        
        $or = array();
        $lines = explode("\n", $slider_categories);
        foreach($lines as $line)
        {
            if( substr($line, 0, 1) == "#" ) continue;
            
            list($slug, $ttl) = explode(" - ", $line);
            $slug = trim($slug);
            $ttl  = trim($ttl); if( empty($ttl) ) $ttl = 0;
            $now  = date("Y-m-d H:i:s");
            
            if( $ttl == 0 )
            {
                $or[] = "main_category in (select id_category from categories where categories.slug = '$slug')";
            }
            else
            {
                $boundary = date("Y-m-d H:i:s", strtotime("now - $ttl hours"));
                $or[] = "(
                           main_category in (select id_category from categories where categories.slug = '$slug')
                           and posts.publishing_date >= '$boundary'
                    )";
            }
        }
        
        $return->where[] = "\n# Slider posts start\n(\n" . implode("\nOR\n", $or) . "\n# Slider posts end\n)";
        
        return $return;
    }
    
    /**
     * @param $id_category
     *
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params_for_category($id_category)
    {
        $return = $this->build_find_params();
        
        $return->where[]
            = "( main_category = '{$id_category}' or id_post in
                 ( select id_post from post_categories
                   where post_categories.id_category = '{$id_category}' )
               )";
        
        return $return;
    }
    
    public function build_find_params_for_date_archive($start_date, $end_date)
    {
        $return = $this->build_find_params(array(), true);
    
        $return->where[] = "publishing_date >= '$start_date'";
        $return->where[] = "publishing_date <= '$end_date'";
        
        return $return;
    }
    
    /**
     * @param $id_account
     *
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params_for_author($id_account)
    {
        $return = $this->build_find_params();
        
        $return->where[] = "id_author = '$id_account'";
        
        return $return;
    }
    
    /**
     * @param $tag
     *
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params_for_tag($tag)
    {
        $return = $this->build_find_params();
        
        $return->where[]
            = "( id_post in (select id_post from post_tags where post_tags.tag = '{$tag}') )";
        
        return $return;
    }
    
    /**
     * @param bool $pinned_first
     *
     * @return posts_data
     */
    public function get_for_home($pinned_first = false)
    {
        global $settings;
        
        $find_params = $this->build_find_params_for_home();
        if( $pinned_first ) $find_params->order = "pin_to_home desc, publishing_date desc";
        $posts_data = $this->get_posts_data($find_params, "index_builders", "home");
        
        $find_params = $this->build_find_params_for_featured_posts();
        if( $pinned_first ) $find_params->order = "pin_to_home desc, publishing_date desc";
        $posts_data->featured_posts = $this->find($find_params->where, $find_params->limit, $find_params->offset, $find_params->order);
        
        $posts_data->slider_posts = array();
        if( $settings->get("modules:posts.slider_categories") != "" )
        {
            $find_params = $this->build_find_params_for_posts_slider();
            if( $pinned_first ) $find_params->order = "pin_to_home desc, publishing_date desc";
            $posts_data->slider_posts = $this->find($find_params->where, $find_params->limit, $find_params->offset, $find_params->order);
        }
        
        return $posts_data;
    }
    
    /**
     * @param $id_account
     *
     * @return posts_data
     */
    public function get_for_author($id_account)
    {
        $find_params = $this->build_find_params_for_author($id_account);
        
        return $this->get_posts_data($find_params, "index_builders", "author_index");
    }
    
    /**
     * @param      $id_category
     * @param bool $pinned_first
     *
     * @return posts_data
     */
    public function get_for_category($id_category, $pinned_first = false)
    {
        $find_params = $this->build_find_params_for_category($id_category);
    
        if( $pinned_first ) $find_params->order = "pin_to_main_category_index desc, publishing_date desc";
        
        return $this->get_posts_data($find_params, "index_builders", "category_index");
    }
    
    /**
     * @param $start_date
     * @param $end_date
     *
     * @return posts_data
     */
    public function get_for_date_range($start_date, $end_date)
    {
        $find_params = $this->build_find_params_for_date_archive($start_date, $end_date);
        
        return $this->get_posts_data($find_params, "index_builders", "date_archive");
    }
    
    /**
     * @param $tag
     *
     * @return posts_data
     */
    public function get_for_tag($tag)
    {
        $find_params = $this->build_find_params_for_tag($tag);
        
        return $this->get_posts_data($find_params, "index_builders", "tag_index");
    }
    
    /**
     * Standard way to build the posts collection 
     * 
     * @param array  $where
     * @param int    $limit
     * @param int    $offset
     * @param string $order
     *
     * @return post_record[]
     */
    public function lookup($where, $limit = 0, $offset = 0, $order = "")
    {
        $params = $this->build_find_params();
        
        if( empty($where)  ) $where  = array();
        if( empty($limit)  ) $limit  = $params->limit;
        if( empty($offset) ) $offset = $params->offset;
        if( empty($order)  ) $order  = $params->order;
        
        $where = array_merge($where, $params->where);
        
        return parent::find($where, $limit, $offset, $order);
    }
    
    /**
     * @param $find_params
     * @param string $extensions_hook   optional
     * @param string $extensions_marker optional
     *
     * @return posts_data
     */
    protected function get_posts_data($find_params, $extensions_hook, $extensions_marker)
    {
        global $modules, $config;
        
        $posts_data = new posts_data();
        
        $posts_data->browser     = new record_browser("");
        $posts_data->count       = $this->get_record_count($find_params->where);
        $posts_data->pagination  = $posts_data->browser->build_pagination($posts_data->count, $find_params->limit, $find_params->offset);
        $posts_data->posts       = $this->find($find_params->where, $find_params->limit, $find_params->offset, $find_params->order);
        
        $this->preload_authors($posts_data);
        
        $config->globals["posts_data"] = $posts_data;
        if( ! empty($extensions_hook) )
            $modules["posts"]->load_extensions($extensions_hook, $extensions_marker);
        
        return $posts_data;
    }
    
    private function preload_authors(posts_data &$posts_data)
    {
        $author_ids = array();
        foreach($posts_data->posts          as $post) $author_ids[] = $post->id_author;
        foreach($posts_data->featured_posts as $post) $author_ids[] = $post->id_author;
        if( count($author_ids) > 0 )
        {
            $author_ids = array_unique($author_ids);
            $authors_repository = new accounts_repository();
            $authors = $authors_repository->get_multiple($author_ids);
        
            foreach($posts_data->posts as $index => &$post)
                $post->set_author($authors[$post->id_author]);
        
            foreach($posts_data->featured_posts as $index => &$post)
                $post->set_author($authors[$post->id_author]);
        }
    }
    
    public function get_grouped_tag_counts($since = "", $min_hits = 10)
    {
        global $database;
        
        $min_hits = empty($min_hits) ? 10 : $min_hits;
        $having   = $min_hits == 1   ? "" : "having `count` >= '$min_hits'";
        
        if( empty($since) )
            $query = "
                select tag, count(tag) as `count` from post_tags
                group by tag
                $having
                order by `count` desc
            ";
        else
            $query = "
                select tag, count(tag) as `count` from post_tags
                where date_attached >= '{$since}'
                group by tag
                $having
                order by `count` desc
            ";
        
        $res = $database->query($query);
        $this->last_query = $database->get_last_query();
        if( $database->num_rows($res) == 0 ) return array();
        
        $return = array();
        while( $row = $database->fetch_object($res) )
            $return[$row->tag] = $row->count;
        
        return $return;
    }
    
    /**
     * @param bool $with_counts
     *
     * @return array 2-dimensional: year, month
     */
    public function get_archive_dates($with_counts = true)
    {
        global $database;
        
        $find_params = $this->build_find_params();
    
        $counts_addition = $with_counts
            ? ", count(id_post) as `count`"
            : "";
        
        $where = parent::convert_where($find_params->where);
        $res = $database->query("
            select
                year(publishing_date) as `year`,
                month(publishing_date) as `month`
                $counts_addition
            from {$this->table_name}
            where $where
            group by
                year(publishing_date),
                month(publishing_date)
            order by
                year(publishing_date) desc,
                month(publishing_date) desc
        ");
        $this->last_query = $database->get_last_query();
        if( $database->num_rows($res) == 0 ) return array();
        
        $return = array();
        while($row = $database->fetch_object($res))
            $return[$row->year][$row->month] = $row->count;
        
        return $return;
    }
    
    public function change_status($id_post, $new_status)
    {
        global $database;
        
        $res = $database->exec("update {$this->table_name} set status = '$new_status' where  id_post = '$id_post'");
        $this->last_query = $database->get_last_query();
        
        return $res;
    }
    
    public function add_tag($id_post, $tag, $date, $order)
    {
        global $database;
        
        $res = $database->exec("
            insert ignore into post_tags set
            id_post        = '$id_post',
            tag            = '$tag',
            date_attached  = '$date',
            order_attached = '$order'
        ");
        $this->last_query = $database->get_last_query();
        
        return $res;
    }
    
    public function delete_tag($id_post, $tag)
    {
        global $database;
        
        $res = $database->exec("
            delete from post_tags where
            id_post = '$id_post' and
            tag     = '$tag'
        ");
        $this->last_query = $database->get_last_query();
        
        return $res;
    }
}
