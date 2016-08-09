<?php
/**
 * Post editing form
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var \SimpleXMLElement $language
 */

use hng2_modules\categories\categories_repository;

$categories_repository = new categories_repository();
$default_category      = $categories_repository->get("0000000000000");

$tree_filter = array(
    "id_category <> '0000000000000'",
    "( visibility = 'public' or visibility = 'users' or (visibility  = 'level_based' and '{$account->level}' >= min_level) )"
);
$categories_count = $categories_repository->get_record_count($tree_filter);
?>

<div id="post_form_target" style="display: none;"></div>

<form name="post_form" id="post_form" method="post"
      action="<?= $config->full_root_path ?>/posts/scripts/save.php">
    
    <input type="hidden" name="id_post" value="">
    <input type="hidden" name="status" value="draft">
    
    <div class="field" data-field="title">
        <textarea name="title" class="expandible_textarea no_fw_font" 
                  placeholder="<?= $current_module->language->form->fields->title->placeholder ?>"></textarea>
    </div>
    
    <? $style = $categories_count == 0 ? "display: none" : ""; ?>
    <div class="field" id="main_category_selector_container" data-field="category" style="<?= $style ?>">
        <select name="main_category" class="fixed_font" style="font-size: 12pt;">
            <option selected value="<?= $default_category->id_category ?>">
                <?= $default_category->title ?>: <?= $default_category->description ?>
            </option>
        </select>
    </div>
    
    <div class="field tinymce_container" data-field="content">
        <textarea name="content" id="post_content_editor" class="tinymce_full"
                  placeholder="<?= $current_module->language->form->fields->content->placeholder ?>"></textarea>
    </div>
    
    
    <div class="field framed_content" data-field="controls" style="display: none;">
        <div class="multicol cols-3 biggest-col">
            <div class="col subfield">
                <div class="caption"><?= $current_module->language->form->fields->excerpt->caption ?></div>
                <div class="input">
                <textarea name="excerpt" class="expandible_textarea no_fw_font" style="min-height: 150px;"
                          placeholder="<?= $current_module->language->form->fields->excerpt->placeholder ?>"></textarea>
                </div>
            </div>
            
            <div class="col subfield">
                <div class="caption"><?= $current_module->language->form->fields->visibility->caption ?></div>
                <div class="input">
                    <select name="visibility">
                        <? foreach($current_module->language->form->fields->visibility->options->option as $option): ?>
                            <option value="<?= $option["value"] ?>"><?= trim($option) ?></option>
                        <? endforeach; ?>
                    </select>
                </div>
                
                <div class="caption"><?= $current_module->language->form->fields->password->caption ?></div>
                <div class="input">
                    <input type="password" name="password" placeholder="<?= $current_module->language->form->fields->password->placeholder ?>">
                </div>
                
                <div class="caption"><?= $current_module->language->form->fields->allow_comments->caption ?></div>
                <div class="input">
                    <?= $language->no ?>
                    <span class='fa-pseudo-switch' data-input-name='allow_comments' data-value-on='1' data-value-off='0' onclick='toggle_fa_pseudo_switch(this);'>
                        <input type='hidden' name='allow_comments' value='1'>
                        <span class='toggler toggle-on  fa fa-toggle-on'  style=''></span>
                        <span class='toggler toggle-off fa fa-toggle-off' style='display: none'></span>
                    </span>
                    <?= $language->yes ?>
                </div>
            </div>
            
            <div class="col subfield featured_image">
                <input type="hidden" name="id_featured_image" value="">
                <div class="caption">
                    <?= $current_module->language->form->fields->featured_image->caption ?>
                </div>
                <div class="input">
                    <div class="thumbnail preview featured_image pseudo_link" 
                         onclick="set_post_featured_image()">
                        <img src="<?= $config->full_root_path ?>/media/missing_image.png"
                             data-empty-src="<?= $config->full_root_path ?>/media/missing_image.png">
                    </div>
                    <div align="center">
                        <span class="pseudo_link" onclick="set_post_featured_image()">
                            <?= $current_module->language->form->fields->featured_image->set ?>
                        </span>
                        &nbsp;
                        <span class="pseudo_link" onclick="remove_post_featured_image()">
                            <?= $current_module->language->form->fields->featured_image->remove ?>
                        </span>
                    </div>
                </div>
            </div>
        </div>
    </div>
    
    <div class="post_buttons alignright">
        <button onclick="$('#post_form').find('.field[data-field=controls]').toggle(); return false;"
                title="<?= $current_module->language->form->fields->controls_toggle->title ?>">
            <span class="fa fa-check-square-o"></span>
            <?= $current_module->language->form->fields->controls_toggle->caption ?>
        </button>
        
        <button type="reset" onclick="hide_post_form()">
            <span class="fa fa-ban"></span>
            <?= $language->cancel ?>
        </button>
        
        <button type="submit" data-save-type="save_draft"
                onclick="$('#post_form').find('input[name=status]').val('draft')">
            <span class="fa fa-save"></span>
            <?= $current_module->language->form->buttons->save_draft ?>
        </button>
        <button type="submit" data-save-type="save">
            <span class="fa fa-save"></span>
            <?= $language->save ?>
        </button>
        <button type="submit" data-save-type="publish"
                onclick="$('#post_form').find('input[name=status]').val('published')">
            <?= $current_module->language->form->buttons->publish ?>
            <span class="fa fa-play"></span>
        </button>
    </div>
</form>
