<?php
namespace hng2_modules\posts;

use hng2_repository\abstract_record;

class post_tag extends abstract_record
{
    public $id_post;
    public $tag;
    public $date_attached;
    public $order_attached;
    
    public function set_new_id()
    {
    }
}
