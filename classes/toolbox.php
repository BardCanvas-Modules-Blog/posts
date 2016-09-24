<?php
namespace hng2_modules\posts;

use hng2_modules\categories\categories_repository;

class toolbox
{
    /**
     * @var categories_repository
     */
    private $categories_repository;
    
    private $categories_by_slug;
    
    public function __construct()
    {
        $this->categories_repository = new categories_repository();
    }
    
    /**
     * Returns a list of messages per category id
     * 
     * @return array [id => message, ...]
     */
    public function get_per_category_messages()
    {
        global $settings;
        
        if( empty($this->categories_by_slug) )
            $this->categories_by_slug = $this->categories_repository->get_ids_by_slug();
        
        $messages = $settings->get("modules:posts.per_category_messages");
        if( empty($messages) ) return array();
        
        $list_by_id = array();
        
        $lines = explode("\n", trim($messages));
        foreach($lines as $line)
        {
            $line = trim($line);
            if( substr($line, 0, 1) == "#" ) continue;
            
            list($slug, $text) = explode("|", $line);
            $slug = trim($slug);
            $text = trim($text);
            
            if( empty($this->categories_by_slug[$slug]) ) continue;
            
            $id = $this->categories_by_slug[$slug];
            
            $list_by_id[$id][] = $text;
        }
        
        $return = array();
        foreach($list_by_id as $id => $lines) $return[$id] = implode("<br>\n", $lines);
        
        return $return;
    }
}
