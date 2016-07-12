<?php
namespace hng2_modules\posts;

use hng2_base\repository\abstract_repository;

class posts_repository extends abstract_repository
{
    protected $row_class       = "hng2_modules\\posts\\post_record";
    protected $table_name      = "posts";
    protected $key_column_name = "id_post";
    protected $additional_select_fields = array(
        "( select concat(user_name, '\\t', display_name, '\\t', email, '\\t', level)
           from account where account.id_account = posts.id_author )
           as _author_data",
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
     * @param post_record $record
     *
     * @return int
     */
    public function save($record)
    {
        global $database;
        
        $this->validate_record($record);
        $obj = $record->get_for_database_insertion();
        
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
                views            ,
                comments_count   ,
                
                last_update      ,
                last_viewed      ,
                last_commented   ,
                
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
                '{$obj->views            }',
                '{$obj->comments_count   }',
                
                '{$obj->last_update      }',
                '{$obj->last_viewed      }',
                '{$obj->last_commented   }',
                
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
                
                publishing_date   = '{$obj->publishing_date  }',
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
}
