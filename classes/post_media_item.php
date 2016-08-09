<?php
namespace hng2_modules\posts;

use hng2_repository\abstract_record;

class post_media_item extends abstract_record
{
    public $id_post;
    public $id_media;
    public $date_attached;
    public $order_attached;
    
    public function set_new_id()
    {
    }
}
