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
?>

<script type="text/javascript" src="<?= $config->full_root_path ?>/lib/jquery.exptextarea.js"></script>
<script type="text/javascript" src="<?= $config->full_root_path ?>/posts/media/quick_post_functions~v<?=$config->scripts_version?>.js"></script>

<div id="quick_post_form_container">
    
    <div id="post_form_target" style="display: none;"></div>
    
    <form name="post_form" id="post_form" method="post"
          action="<?= $config->full_root_path ?>/posts/scripts/save.php" enctype="multipart/form-data">
        
        <input type="hidden" name="id_post"       value="">
        <input type="hidden" name="status"        value="published">
        <input type="hidden" name="ok_with_url"   value="true">
        <input type="hidden" name="is_quick_post" value="true">
        
        <div class="field" data-field="title">
            <textarea name="title" class="expandible_textarea no_fw_font"
                      placeholder="<?= $this_module->language->form->fields->title->quick_form_placeholder ?>"
                      data-expanded-placeholder="<?= $this_module->language->form->fields->title->quick_form_expanded_placeholder ?>"
                      onfocus="$(this).closest('form').find('.rest').show(); $(this).attr('placeholder', $(this).attr('data-expanded-placeholder')); hook_abandon_post();"></textarea>
        </div>
        
        <div class="rest" style="display: none;">
            <? if( $template->get("current_category_id") ): ?>
                
                <input type="hidden" name="main_category" value="<?= $template->get("current_category_id") ?>">
                
            <? else:
                $categories_repository = new categories_repository();
                $default_category      = $categories_repository->get("0000000000000");
                
                $mem_key     = "quick_post_categories/level:{$account->level}";
                $res         = $mem_cache->get($mem_key);
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
                    $categories = $categories_repository->get_as_tree_for_select($tree_filter, "", true);
                    if( empty($categories) ) $mem_cache->set($mem_key, "none", 0, 60*60);
                    else                     $mem_cache->set($mem_key, $categories, 0, 60*60);
                }
                $categories_count = count($categories);
                $style = $categories_count == 0 ? "display: none" : "";
                ?>
                <div class="field" id="main_category_selector_container" data-field="category" style="<?= $style ?>">
                    <select name="main_category" class="big_monospace_select">
                        <option selected value="<?= $default_category->id_category ?>">
                            <?= $default_category->title ?>: <?= $default_category->description ?>
                        </option>
                        <? foreach($categories as $id_category => $description): ?>
                            <option value="<?= $id_category ?>"><?= $description ?></option>
                        <? endforeach; ?>
                    </select>
                </div>
            <? endif; ?>
            
            <? $modules["posts"]->load_extensions("quick_post_form", "below_main_category_selector"); ?>
            
            <div class="field tinymce_container" data-field="content">
                <textarea name="content" id="post_content_editor" class="tinymce"
                          placeholder="<?= $this_module->language->form->fields->content->quick_form_placeholder ?>"></textarea>
            </div>
            
            <div class="field attachments"></div>
            
            <div class="field post_buttons clearfix">
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
