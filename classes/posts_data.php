<?php
namespace hng2_modules\posts;

use hng2_tools\record_browser;

class posts_data
{
    /**
     * @var record_browser
     */
    public $browser = null;
    
    /**
     * @var int
     */
    public $count = 0;
    
    /**
     * @var array
     */
    public $pagination = array();
    
    /**
     * @var post_record[]
     */
    public $posts = array();
    
    /**
     * @var post_record[]
     */
    public $featured_posts = array();
    
    /**
     * @var post_record[]
     */
    public $slider_posts = array();
}
