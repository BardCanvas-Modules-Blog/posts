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

use hng2_base\config;
use hng2_modules\categories\categories_repository;
use hng2_modules\posts\toolbox;

$posts_toolbox         = new toolbox();
$categories_repository = new categories_repository();
$default_category      = $categories_repository->get("0000000000000");

$tree_filter = array(
    "id_category <> '0000000000000'",
    "( visibility = 'public' or visibility = 'users' or (visibility  = 'level_based' and '{$account->level}' >= min_level) )"
);
$categories_count = $categories_repository->get_record_count($tree_filter);
?>

<div id="post_form_target" style="display: none;"></div>

<form name="post_form" id="post_form" method="post" data-preview-mode=""
      action="<?= $config->full_root_path ?>/posts/scripts/save.php">
    
    <input type="hidden" name="id_post" value="">
    <input type="hidden" name="status" value="draft">
    
    <div class="field" data-field="title">
        <textarea name="title" class="expandible_textarea no_fw_font" 
                  placeholder="<?= $current_module->language->form->fields->title->placeholder ?>"></textarea>
    </div>
    
    <? $style = $categories_count == 0 ? "display: none" : ""; ?>
    <div class="field" id="main_category_selector_container" data-field="category" style="<?= $style ?>">
        <select name="main_category" class="big_monospace_select"  onchange="toggle_post_category_message(this)">
            <option selected value="<?= $default_category->id_category ?>">
                <?= $default_category->title ?>: <?= $default_category->description ?>
            </option>
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
    
    <? $modules["posts"]->load_extensions("editor_form", "below_main_category_selector"); ?>
    
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
                            <option value="<?= $option["value"] ?>">
                                <?= replace_escaped_vars(trim($option), '{$level}', $config->user_levels_by_level[$account->level] ) ?>
                            </option>
                        <? endforeach; ?>
                    </select>
                </div>
    
                <? $info_handler = "post_composer_password_info"; ?>
                <div class="caption">
                    <?= $current_module->language->form->fields->password->caption ?>
                    <i class="pseudo_link" onclick="toggle_info_section('<?= $info_handler ?>')"><i
                       class="fa fa-info-circle" title="<?= $language->toggle_info ?>"></i></i>
                </div>
                <div class="input info_handler" id="<?= $info_handler ?>"
                     style="<? if($account->engine_prefs[$info_handler] == "hidden") echo "display: none;"; ?>">
                    <i class="fa fa-info-circle"></i>
                    <?= $current_module->language->form->fields->password->info ?>
                </div>
                <div class="input">
                    <input type="text" name="password" placeholder="<?= $current_module->language->form->fields->password->placeholder ?>">
                </div>
                
                <div class="field" data-field="slug">
                    <div class="caption">
                        <?= $current_module->language->form->fields->slug->caption ?>
                    </div>
                    <div class="input">
                        <input type="text" name="slug" value="" maxlength="128"
                               placeholder="<?= $current_module->language->form->fields->slug->placeholder ?>">
                    </div>
                </div>
                
                <? if( $account->level >= config::MODERATOR_USER_LEVEL ): ?>
                    <div class="caption"><?= $current_module->language->form->fields->allow_comments->caption ?></div>
                    <div class="input">
                        <?= $language->no ?>
                        <span class='fa-pseudo-switch' data-input-name='allow_comments' data-value-on='1' data-value-off='0' onclick='toggle_fa_pseudo_switch(this);'>
                        <input type='hidden' name='allow_comments' value='1'>
                        <span class='toggler toggle-on  fa fa-toggle-on'  style='display: none'></span>
                        <span class='toggler toggle-off fa fa-toggle-off' style=''></span>
                    </span>
                        <?= $language->yes ?>
                    </div>
                    
                    <div class="caption"><?= $current_module->language->form->fields->pin_to_home->caption ?></div>
                    <div class="input">
                        <?= $language->no ?>
                        <span class='fa-pseudo-switch' data-input-name='pin_to_home' data-value-on='1' data-value-off='0' onclick='toggle_fa_pseudo_switch(this);'>
                            <input type='hidden' name='pin_to_home' value='0'>
                            <span class='toggler toggle-on  fa fa-toggle-on'  style=''></span>
                            <span class='toggler toggle-off fa fa-toggle-off' style='display: none'></span>
                        </span>
                        <?= $language->yes ?>
                    </div>
                    
                    <div class="caption"><?= $current_module->language->form->fields->pin_to_main_category_index->caption ?></div>
                    <div class="input">
                        <?= $language->no ?>
                        <span class='fa-pseudo-switch' data-input-name='pin_to_main_category_index' data-value-on='1' data-value-off='0' onclick='toggle_fa_pseudo_switch(this);'>
                            <input type='hidden' name='pin_to_main_category_index' value='0'>
                            <span class='toggler toggle-on  fa fa-toggle-on'  style=''></span>
                            <span class='toggler toggle-off fa fa-toggle-off' style='display: none'></span>
                        </span>
                        <?= $language->yes ?>
                    </div>
                <? endif; ?>
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
    
    <div class="post_buttons alignright clearfix">
        <span class="post_autosave_status pull-left">
            <span class="saving" style="display: none">
                <span class="fa fa-spinner fa-pulse"></span>
                <?= $current_module->language->saving ?>
            </span>
            <span class="saved" style="display: none">
                <span class="fa fa-check"></span>
                <?= $current_module->language->saved ?>
            </span>
            <span class="error" style="display: none;">
                <span class="fa fa-warning"></span>
                <span class="message"></span>
            </span>
        </span>
        
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
                onclick="var $form = $('#post_form'); $form.find('input[name=status]').val('draft'); $form.attr('data-prevew-mode', 'true');">
            <span class="fa fa-search"></span>
            <?= $current_module->language->form->buttons->preview ?>
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
