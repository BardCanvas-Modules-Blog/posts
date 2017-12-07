<?php
/**
 * Quick post form - to be embedded on the frontend (called from the template)
 * 
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var config     $config
 * @var posts_data $posts_data
 * @var template   $template
 * @var module[]   $modules
 * @var module     $this_module posts
 */

global $mem_cache;

use hng2_base\config;
use hng2_base\module;
use hng2_base\template;
use hng2_modules\categories\categories_repository;
use hng2_modules\posts\posts_data;
use hng2_modules\posts\toolbox;

$posts_toolbox = new toolbox();

$class = "";
if( isset($posts_data->slider_posts) )
    if( count($posts_data->slider_posts) == 1 )
        $class = "spaced";

if( $current_module->name == "posts" ) $this_module = $current_module;

$categories_repository = new categories_repository();
if( $template->get("current_category_id") )
{
    $category = $categories_repository->get($template->get("current_category_id"));
    if( $category->visibility == "level_based" && $account->level < $category->min_level )
        return;
}

$config->globals["show_quick_post_form"] = true;
$this_module->load_extensions("quick_post_form", "pre_rendering");
if( ! $config->globals["show_quick_post_form"] ) return;

#region hiding the quick posts form based on main category selector exceptions
$raw_list = $settings->get("modules:posts.main_category_selector_exceptions");
if( ! empty($raw_list) )
{
    $page_cat = $template->get("current_category_slug");
    $page_tag = $template->get("page_tag");
    $lines    = explode("\n", $raw_list);
    foreach($lines as $line)
    {
        $line = trim($line);
        if( empty($line) ) continue;
        
        list($slug, $level) = preg_split('/\s+-\s+/', $line);
        
        if( $page_tag == "post_category_index" && $slug == $page_cat && $account->level < $level )
        {
            $config->globals["show_quick_post_form"] = false;
            
            break;
        }
    }
}

if( ! $config->globals["show_quick_post_form"] ) return;
#endregion
?>

<script type="text/javascript"> var saved_as_popup_message = '<?= htmlspecialchars(trim($current_module->language->messages->saved_as_popup)) ?>'; </script>
<script type="text/javascript" src="<?= $config->full_root_path ?>/lib/jquery.exptextarea.js"></script>
<script type="text/javascript" src="<?= $this_module->get_url() ?>/media/quick_post_functions~v1.3.0-2.js"></script>

<div id="quick_post_form_container" class="<?= $class ?>">
    
    <div id="post_form_target" style="display: none;"></div>
    
    <form name="post_form" id="post_form" method="post"
          action="<?= $config->full_root_path ?>/posts/scripts/save.php" enctype="multipart/form-data">
        
        <input type="hidden" name="id_post"       value="">
        <input type="hidden" name="status"        value="published">
        <input type="hidden" name="ok_with_url"   value="true">
        <input type="hidden" name="is_quick_post" value="true">
        <input type="hidden" name="as_popup"      value="<?= trim(stripslashes($_GET["as_popup"])) ?>">
        
        <div class="field" data-field="title">
            <textarea name="title" class="expandible_textarea no_fw_font"
                      placeholder="<?= $this_module->language->form->fields->title->quick_form_placeholder ?>"
                      data-expanded-placeholder="<?= $this_module->language->form->fields->title->quick_form_expanded_placeholder ?>"
                      onfocus="expand_quick_post_area()"></textarea>
        </div>
        
        <div class="rest" style="display: none;">
            <? if( $template->get("current_category_id") ): ?>
                
                <input type="hidden" name="main_category" value="<?= $template->get("current_category_id") ?>">
                
            <? else:
                $default_category = $categories_repository->get("0000000000000");
                
                $mem_key = "categories:for_quick_posts_form/level:{$account->level}";
                $res     = $mem_cache->get($mem_key);
                if( is_array($res) )
                {
                    $categories = $res;
                }
                elseif( $res == "none" )
                {
                    $categories = array();
                }
                else
                {
                    $tree_filter = array("id_category <> '0000000000000'");
                    
                    $raw_list = $settings->get("modules:posts.main_category_selector_exceptions");
                    if( ! empty($raw_list) )
                    {
                        $exception_slugs = array();
                        foreach(explode("\n", trim($raw_list)) as $line)
                        {
                            $line = trim($line);
                            if( empty($line) ) continue;
                            
                            list($slug, $level) = preg_split('/\s+-\s+/', $line);
                            if( $account->level >= $level ) continue;
                            
                            $tree_filter[] = "slug <> '$slug'";
                        }
                    }
                    
                    $categories = $categories_repository->get_as_tree_for_select($tree_filter, "", true);
                    if( empty($categories) ) $mem_cache->set($mem_key, "none",      0, 60 * 60);
                    else                     $mem_cache->set($mem_key, $categories, 0, 60 * 60);
                }
                $categories_count = count($categories);
                $style = $categories_count == 0 ? "display: none" : "";
                
                $options = array();
                foreach($categories as $id_category => $description)
                    $options[$id_category] = "<option value='$id_category'>$description</option>";
                
                $config->globals["modules:posts.prebuilt_category_selector_options"] =& $options;
                $this_module->load_extensions("quick_post_form", "process_prebuilt_category_selector_options");
                ?>
                <div class="field" id="main_category_selector_container" data-field="category" style="<?= $style ?>">
                    <select name="main_category" class="big_monospace_select" onchange="toggle_post_category_message(this)">
                        <option selected value="<?= $default_category->id_category ?>">
                            <?= $default_category->title ?>: <?= $default_category->description ?>
                        </option>
                        <?= implode("\n                        ", $options) ?>
                    </select>
                    
                    <div class="category_messages">
                        <? foreach( $posts_toolbox->get_per_category_messages() as $category_id => $text ): ?>
                            <div class="message framed_content state_highlight" style="display: none;"
                                 data-category-id="<?= $category_id ?>">
                                <span class="fa fa-info-circle"></span>
                                <?= $text ?>
                            </div>
                        <? endforeach; ?>
                    </div>
                </div>
            <? endif; ?>
            
            <? $modules["posts"]->load_extensions("quick_post_form", "below_main_category_selector"); ?>
            
            <div class="field tinymce_container" data-field="content" data-tinymce-hidden-buttons="gallery-module">
                <textarea name="content" id="post_content_editor" class="tinymce"
                          placeholder="<?= $this_module->language->form->fields->content->quick_form_placeholder ?>"></textarea>
            </div>
            
            <div class="field attachments"></div>
            
            <div class="field post_buttons clearfix">
                <? if( $_GET["no_attachments"] != "true" ): ?>
                    <span>
                        <button onclick="add_attachment_to_quick_post('image', 'fa-camera'); return false;">
                            <span class="fa fa-camera"></span>
                            <?= $this_module->language->form->fields->buttons->image ?>
                        </button>
                        <button onclick="add_attachment_to_quick_post('video', 'fa-video-camera'); return false;">
                            <span class="fa fa-video-camera"></span>
                            <?= $this_module->language->form->fields->buttons->video ?>
                        </button>
                    </span>
                <? endif; ?>
                
                <span style="float: right">
                    <button type="submit" data-save-type="save_draft"
                            onclick="$('#post_form').find('input[name=status]').val('draft');">
                        <span class="fa fa-save"></span>
                        <?= $this_module->language->form->buttons->save_draft ?>
                    </button>
                    
                    <button type="submit" data-save-type="publish">
                        <?= $this_module->language->form->buttons->publish ?>
                        <span class="fa fa-play"></span>
                    </button>
                </span>
            </div>
        </div>
        
    </form>
    
</div>

<? if( $_GET["trigger_quick_post_form"] == "true" ): ?>
    <script type="text/javascript">
        expand_quick_post_area();
        $.scrollTo('#quick_post_form_container', 1000);
    </script>
<? endif; ?>
