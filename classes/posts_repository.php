<?php
namespace hng2_modules\posts;

use hng2_base\repository\abstract_repository;

class posts_repository extends abstract_repository
{
    protected $row_class       = "hng2_modules\\posts\\post_record";
    protected $table_name      = "posts";
    protected $key_column_name = "id_post";
    
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
                tags_cache       ,
                categories_cache ,
                media_cache      ,
                mentions_cache   ,
                visibility       ,
                status           ,
                password         ,
                creation_date    ,
                creation_host    ,
                creation_location,
                creation_details ,
                publishing_date  ,
                views            ,
                allow_comments   ,
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
                '{$obj->tags_cache       }',
                '{$obj->categories_cache }',
                '{$obj->media_cache      }',
                '{$obj->mentions_cache   }',
                '{$obj->visibility       }',
                '{$obj->status           }',
                '{$obj->password         }',
                '{$obj->creation_date    }',
                '{$obj->creation_host    }',
                '{$obj->creation_location}',
                '{$obj->creation_details }',
                '{$obj->publishing_date  }',
                '{$obj->views            }',
                '{$obj->allow_comments   }',
                '{$obj->comments_count   }',
                '{$obj->last_update      }',
                '{$obj->last_viewed      }',
                '{$obj->last_commented   }',
                '{$obj->id_featured_image}'
            ) on duplicate key update
                parent_post       = '{$obj->parent_post      }',
                id_author         = '{$obj->id_author        }',
                slug              = '{$obj->slug             }',
                title             = '{$obj->title            }',
                excerpt           = '{$obj->excerpt          }',
                content           = '{$obj->content          }',
                tags_cache        = '{$obj->tags_cache       }',
                categories_cache  = '{$obj->categories_cache }',
                media_cache       = '{$obj->media_cache      }',
                mentions_cache    = '{$obj->mentions_cache   }',
                visibility        = '{$obj->visibility       }',
                status            = '{$obj->status           }',
                password          = '{$obj->password         }',
                creation_date     = '{$obj->creation_date    }',
                creation_host     = '{$obj->creation_host    }',
                creation_location = '{$obj->creation_location}',
                creation_details  = '{$obj->creation_details }',
                publishing_date   = '{$obj->publishing_date  }',
                views             = '{$obj->views            }',
                allow_comments    = '{$obj->allow_comments   }',
                comments_count    = '{$obj->comments_count   }',
                last_update       = '{$obj->last_update      }',
                last_viewed       = '{$obj->last_viewed      }',
                last_commented    = '{$obj->last_commented   }',
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
