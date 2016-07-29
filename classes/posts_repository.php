<?php
namespace hng2_modules\posts;

use hng2_base\repository\abstract_repository;
use hng2_base\repository\accounts_repository;
use hng2_modules\categories\category_record;
use hng2_tools\record_browser;

class posts_repository extends abstract_repository
{
    protected $row_class       = "hng2_modules\\posts\\post_record";
    protected $table_name      = "posts";
    protected $key_column_name = "id_post";
    protected $additional_select_fields = array(
        "( select concat(user_name, '\\t', display_name, '\\t', email, '\\t', level)
           from account where account.id_account = posts.id_author )
           as _author_data",
        "( select concat(slug, '\\t', title)
           from categories where categories.id_category = posts.main_category )
           as _main_category_data",
        "( select thumbnail
           from media where media.id_media = posts.id_featured_image )
           as featured_image_thumbnail",
        
        "( select group_concat(tag order by order_attached asc separator ',')
           from post_tags where post_tags.id_post = posts.id_post )
           as tags_list",
        "( select group_concat(id_media order by order_attached asc separator ',')
           from post_media where post_media.id_post = posts.id_post )
           as media_list",
        "( select group_concat(id_category order by order_attached asc separator ',')
           from post_categories where post_categories.id_post = posts.id_post )
           as categories_list",
        "( select group_concat(id_account order by order_attached asc separator ',')
           from post_mentions where post_mentions.id_post = posts.id_post )
           as mentions_list",
    );
    
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
        
        return $database->exec("
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
                
                expiration_date   = '{$obj->expiration_date  }',
                last_update       = '{$obj->last_update      }',
                id_featured_image = '{$obj->id_featured_image}'
        ");
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
        return $database->exec("
            insert into post_categories set
            id_post        = '$id_post',
            id_category    = '$id_category',
            date_attached  = '$date',
            order_attached = '$order'
        ");
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
        foreach($list as $tag)
        {
            if( ! isset($actual_tags[$tag]) ) $inserts[] = "('$id_post', '$tag', '$date', '". microtime(true) . "')";
            unset($actual_tags[$tag]);
        }
        
        if( ! empty($inserts) )
            $database->exec(
                "insert into post_tags (id_post, tag, date_attached, order_attached) " .
                "values " . implode(", ", $inserts)
            );
        
        if( ! empty($actual_tags) )
        {
            $deletes = array();
            foreach($actual_tags as $tag => $object) $deletes[] = "'$tag'";
            $database->exec("delete from post_tags where tag in (" . implode(", ", $deletes) . ")");
        }
    }
    
    public function unset_category($id_category, $id_post)
    {
        global $database;
        
        return $database->exec("
            delete from post_categories where
            id_post     = '$id_post' and
            id_category = '$id_category'
        ");
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
                order_attached
        ");
        
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
        return $database->exec("
            update posts set
                status      = 'trashed',
                last_update = '$date'
            where
                id_post = '$id_post'
        ");
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
        
        // TODO: Complement where[] with additional filters (per user level, etc.)
        
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
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params_for_home()
    {
        global $settings;
        
        $return = $this->build_find_params();
        
        # Added to EXCLUDE featured posts
        $return->where[]
            = "id_post not in
               (
                   select post_tags.id_post from post_tags
                   where post_tags.id_post = posts.id_post
                   and post_tags.tag = '{$settings->get("modules:posts.featured_posts_tag")}'
               )";
        
        return $return;
    }
    
    /**
     * @return object {where:array, limit:int, offset:int, order:string}
     */
    protected function build_find_params_for_featured_posts()
    {
        global $settings;
        
        // TODO: Add expiration date to posts and implement it here
    
        $return = $this->build_find_params();
    
        # Added to LIST ONLY featured posts
        $return->where[]
            = "id_post in
               (
                 select post_tags.id_post from post_tags
                 where post_tags.id_post = posts.id_post
                 and post_tags.tag = '{$settings->get("modules:posts.featured_posts_tag")}'
               )";
    
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
        
        # Added to EXCLUDE featured posts
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
     * @return posts_data
     */
    public function get_for_home()
    {
        $find_params = $this->build_find_params_for_home();
        
        return $this->get_posts_data($find_params, "index_builders", "home");
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
     * @param $id_category
     *
     * @return posts_data
     */
    public function get_for_category($id_category)
    {
        $find_params = $this->build_find_params_for_category($id_category);
        
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
     * @param $find_params
     * @param $extensions_hook
     * @param $extensions_marker
     *
     * @return posts_data
     */
    private function get_posts_data($find_params, $extensions_hook, $extensions_marker)
    {
        global $modules, $config;
        
        $posts_data = new posts_data();
        
        $posts_data->browser     = new record_browser("");
        $posts_data->count       = $this->get_record_count($find_params->where);
        $posts_data->pagination  = $posts_data->browser->build_pagination($posts_data->count, $find_params->limit, $find_params->offset);
        $posts_data->posts       = $this->find($find_params->where, $find_params->limit, $find_params->offset, $find_params->order);
        
        $this->preload_authors($posts_data);
        
        $config->globals["posts_data"] = $posts_data;
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
}
