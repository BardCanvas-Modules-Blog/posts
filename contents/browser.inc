<?
/**
 * Posts browser
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var \SimpleXMLElement $language
 */

use hng2_base\account;
use hng2_base\config;
use hng2_media\media_repository;
use hng2_modules\categories\categories_repository;
use hng2_modules\posts\post_record;
use hng2_modules\posts\posts_repository;
use hng2_tools\record_browser;

$browser    = new record_browser("{$config->website_key}_{$current_module->name}");
$repository = new posts_repository();

/**
 * @var int    $offset
 * @var int    $limit
 * @var int    $order
 * @var string $search_for
 * @var string $search_author
 * @var string $search_pinned
 * @var string $search_status
 * @var string $search_category
 */
#region Nav filters
$built_vars = $browser->build_vars(20, 4);
foreach($built_vars as $key => $val) $$key = $val;

$where = array();

$search_for = empty($search_for) ? "" : trim($search_for);
if( ! empty($search_for) )
{
    if( strpos($search_for, ",") !== false )
    {
        $search_for_exploded = explode(",", $search_for);
        $where_line          = "";
        foreach( $search_for_exploded as $this_term )
        {
            $this_term = trim($this_term);
            if( ! empty($this_term) )
            {
                if( is_numeric($this_term) )
                {
                    $where_line .= "id_post     =     '{$this_term}'  or ";
                    $where_line .= "parent_post =     '{$this_term}'  or ";
                }
                else
                {
                    $where_line .= "slug        like '%{$this_term}%' or ";
                    $where_line .= "title       like '%{$this_term}%' or ";
                    $where_line .= "excerpt     like '%{$this_term}%' or ";
                    $where_line .= "content     like '%{$this_term}%' or ";
                }
            }
        }
        $where[] = "(" . preg_replace('/ or $/', "", $where_line) . ")";
    }
    else
    {
        $search_for = trim($search_for);
        if( is_numeric($search_for) )
            $where[] = "("
                . "id_post     =     '{$search_for}'  or "
                . "parent_post =     '{$search_for}'     )"
            ;
        else
            $where[] = "("
                . "slug        like '%{$search_for}%' or "
                . "title       like '%{$search_for}%' or "
                . "excerpt     like '%{$search_for}%' or "
                . "content     like '%{$search_for}%'    )"
            ;
    }
}

if( $account->level < config::MODERATOR_USER_LEVEL )
{
    if( $search_status == "hidden" ) $search_status = "";
    
    if( empty($search_for) )
    {
        if( empty($search_status) ) $where[] = "status not in ('trashed', 'hidden')";
        else                        $where[] = "status = '$search_status'";
    }
    else
    {
        if( empty($search_status) ) $where[] = "status not in ('hidden')";
        else                        $where[] = "status = '$search_status'";
    }
    $where["id_author"] = $account->id_account;
}
else
{
    if( ! empty($search_author) ) $where["id_author"] = $_REQUEST["search_author"];
    if( ! empty($search_pinned) ) $where[]            = "pin_to_home > 0 or pin_to_main_category_index > 0";
    if( ! empty($search_status) ) $where[] = "status = '$search_status'";
}

if( ! empty($search_category) ) $where[] = "main_category = '$search_category'";

#endregion

#region Nav pointers
$record_count = $repository->get_record_count($where);
$pagination   = $browser->build_pagination($record_count, $limit, $offset);
#endregion

#region Data grabbing
switch( $order )
{
    case  1: $sqlorder = "publishing_date asc";  break;
    case  2: $sqlorder = "publishing_date desc"; break;
    case  3: $sqlorder = "last_update asc";      break;
    case  4: $sqlorder = "last_update desc";     break;
    case  5: $sqlorder = "views asc";            break;
    case  6: $sqlorder = "views desc";           break;
    case  7: $sqlorder = "comments_count asc";   break;
    case  8: $sqlorder = "comments_count desc";  break;
    case  9: $sqlorder = "creation_date asc";    break;
    case 10: $sqlorder = "creation_date desc";   break;
}

$records = $repository->find($where, $limit, $offset, $sqlorder);
# echo "<pre>" . print_r($repository->get_last_query(), true) . "</pre>";

$current_module->load_extensions("browser", "after_collection_built");
$per_column_function_hooks = array();
$current_module->load_extensions("browser", "per_record_function_hooks");

$media_repository = new media_repository();

$categories_repository = new categories_repository();
$categories            = $categories_repository->get_as_tree_for_select(array(), "title asc");
#endregion
?>

<? if( ! empty($_GET["edit_post"]) ): ?>
    <script type="text/javascript"> $(document).ready(function() { edit_post('<?= $_GET["edit_post"] ?>') }); </script>
<? endif; ?>

<? if( ! empty($_GET["create"]) ): ?>
    <script type="text/javascript"> $(document).ready(function() { prepare_post_addition(); }); </script>
<? endif; ?>

<? if( ! empty($_GET["trash_post"]) ): ?>
    <script type="text/javascript"> $(document).ready(function() { trash_post('<?= $_GET["trash_post"] ?>') }); </script>
<? endif; ?>

<h1 class="clearfix">
    <button id="refresh_posts_browser" class="pull-right multirecord_actions_table_refresher"
            onclick="$('#filter_form').submit()">
        <span class="fa fa-refresh"></span>
        <?= $language->refresh ?>
    </button>
    
    <?= $current_module->language->index->title; ?>
    
    <button onclick="prepare_post_addition()">
        <span class="fa fa-plus-circle"></span>
        <?= $current_module->language->index->buttons->add ?>
    </button>
</h1>

<div class="filtering clearfix">
    
    <script type="text/javascript">
        $(document).ready(function()
        {
            $('#filter_form').keypress(function(e) {
                if( (e.keyCode == 13) && (e.target.type != "textarea") )
                {
                    e.preventDefault();
                    $(this).submit();
                }
            });
        });
    </script>
    
    <form name="filter_form" id="filter_form" action="<?= $_SERVER["PHP_SELF"] ?>?wasuuup=<? echo mt_rand(1, 65535); ?>"
          method="get">
        
        <input type="hidden" name="mode"          value="set_filter">
        <input type="hidden" name="order"         value="<?= $order ?>">
        <input type="hidden" name="offset"        value="<?= $offset ?>">
        
        <span style="float: right;">
            <?= $browser->get_pagination_button("previous", "paginate", $pagination) ?>
            <?= $language->record_nav->page ?>
            <?= $pagination["this_page_number"]; ?>/<?= $pagination["total_pages"]; ?>
            (<?= $pagination["total_records"]; ?> <?= $language->record_nav->entries ?>)
            <?= $browser->get_pagination_button("next", "paginate", $pagination) ?>
        </span>
        
        <?= $language->record_nav->search ?>
        <input type="text" name="search_for"
               value="<?= htmlspecialchars($search_for) ?>" size="37"
               placeholder="<?= $current_module->language->record_nav->search_placeholder ?>">
        
        <select name="search_category" onchange="$('#filter_form').submit();">
            <option <? if(empty($search_category)) echo "selected"; ?> selected value="">
                &lt;<?= $current_module->language->record_nav->filters->any_category ?>&gt;
            </option>
            <? foreach($categories as $id_category => $description): ?>
                <option <? if($search_category == $id_category) echo "selected"; ?> value="<?= $id_category ?>"><?= $description ?></option>
            <? endforeach; ?>
        </select>
        
        <? if( $account->level >= config::MODERATOR_USER_LEVEL ): ?>
            <select name="search_author" onchange="$('#filter_form').submit();">
                <option <? if( empty($search_author) ) echo "selected"; ?> value="">&lt;<?= $current_module->language->record_nav->filters->from_everyone ?>&gt;</option>
                <option <? if( $search_author == $account->id_account ) echo "selected"; ?> value="<?= $account->id_account ?>"><?= $current_module->language->record_nav->filters->mine ?></option>
                <? if( ! empty($search_author) && $search_author != $account->id_account ): $author = new account($search_author); ?>
                    <option selected value="<?= $search_author ?>"><?= replace_escaped_vars(
                            $current_module->language->record_nav->filters->from_user,
                            '{$user}',
                            $author->_exists ? $author->display_name : $search_author
                        ) ?></option>
                <? endif; ?>
            </select>
            
            <select name="search_pinned" onchange="$('#filter_form').submit();">
                <option <? if($search_pinned == "")     echo "selected"; ?> value="">&lt;<?= $current_module->language->record_nav->filters->pinned_or_not ?>&gt;</option>
                <option <? if($search_pinned == "true") echo "selected"; ?> value="true"><?= $current_module->language->record_nav->filters->pinned ?></option>
            </select>
        <? endif; ?>
        
        <span class="inline-block">
            <?= $language->record_nav->show ?>
            <select name="limit" onchange="$('#filter_form').submit();">
                <? foreach(array(5, 10, 20, 30, 40, 50, 100, 200, 300, 400, 500) as $recs): ?>
                    <option <? if($limit == $recs) echo "selected" ?> value="<?= $recs ?>"><?= $recs ?></option>
                <? endforeach; ?>
            </select>
            <?= $language->record_nav->recs_per_page ?>
            
            <button type="submit"><?= $language->record_nav->buttons->apply ?></button>
            <button onclick="reset_filter();"><?= $language->record_nav->buttons->reset ?></button>
        </span>
        
        <div class="framed_radios" style="margin: 5px 0 0 0;">
            <label class="framed_content inlined <? if($search_status == "") echo "state_active"; ?>">
                <input type="radio" name="search_status" <? if($search_status == "") echo "checked"; ?> value=""
                       onchange="$('#filter_form').submit()">
                <?= $current_module->language->record_nav->filters->any_status ?>
            </label>
            
            <label class="framed_content inlined <? if($search_status == "draft") echo "state_active"; ?>">
                <input type="radio" name="search_status" <? if($search_status == "draft") echo "checked"; ?> value="draft"
                       onchange="$('#filter_form').submit()">
                <?= $current_module->language->status_labels->draft ?>
            </label>
            
            <label class="framed_content inlined <? if($search_status == "published") echo "state_active"; ?>">
                <input type="radio" name="search_status" <? if($search_status == "published") echo "checked"; ?> value="published"
                       onchange="$('#filter_form').submit()">
                <?= $current_module->language->status_labels->published ?>
            </label>
            
            <label class="framed_content inlined <? if($search_status == "reviewing") echo "state_active"; ?>">
                <input type="radio" name="search_status" <? if($search_status == "reviewing") echo "checked"; ?> value="reviewing"
                       onchange="$('#filter_form').submit()" id="reviewing_trigger">
                <?= $current_module->language->status_labels->reviewing ?>
            </label>
            
            <? if( $account->level >= config::MODERATOR_USER_LEVEL ): ?>
                <label class="framed_content inlined <? if($search_status == "hidden") echo "state_active"; ?>">
                    <input type="radio" name="search_status" <? if($search_status == "hidden") echo "checked"; ?> value="hidden"
                           onchange="$('#filter_form').submit()">
                    <?= $current_module->language->status_labels->hidden ?>
                </label>
            <? endif; ?>
            
            <label class="framed_content inlined <? if($search_status == "trashed") echo "state_active"; ?>">
                <input type="radio" name="search_status" <? if($search_status == "trashed") echo "checked"; ?> value="trashed"
                       onchange="$('#filter_form').submit()">
                <?= $current_module->language->status_labels->trashed ?>
            </label>
            
            <? if($account->_is_admin && $search_status == "trashed"): ?>
                <button onclick="empty_trash(); return false;" style="font-size: 12pt; padding: 5px;">
                    <span class="fa fa-trash"></span>
                    <?= $current_module->language->empty_trash->caption ?>
                </button>
            <? endif; ?>
            
        </div>
    </form>
</div>

<? if( $record_count == 0 ): ?>
    <div class="framed_content state_ko">
        <span class="fa fa-info-circle"></span>
        <?= $language->record_nav->no_records_found ?>
    </div>
    <? return; ?>
<? endif;  ?>

<?
if( $account->level >= config::MODERATOR_USER_LEVEL && $search_status != "reviewing" ):
    $pending = $repository->get_record_count(array("status" => "reviewing"));
    if( $pending > 0 ):
        $message = replace_escaped_vars($current_module->language->messages->pending, '{$amount}', $pending);
        ?>
        <div class="framed_content state_ko">
            <span class="pseudo_link" onclick="var $form = $('#filter_form'); reset_filter(); $form.find('input[name=search_status][value=reviewing]').prop('checked', true); $form.submit();">
                <span class="fa fa-warning"></span>
                <?= $message ?>
            </span>
        </div>
        <?
    endif;
endif;
?>

<div class="table_wrapper">
    
    <div class="multi_record_actions" style="font-family: arial, helvetica, sans-serif; margin-bottom: 5px;">
        <span class="fixed_font">&nbsp;┌──</span>
        <button onclick="launch_multirecord_actions(this, 'trashed')">
            <?= $current_module->language->multi_actions->trash ?>
        </button>
        <? if( $account->level >= config::MODERATOR_USER_LEVEL ): ?>
            <button onclick="launch_multirecord_actions(this, 'draft')">
                <?= $current_module->language->multi_actions->draft ?>
            </button>
            <button onclick="launch_multirecord_actions(this, 'hidden')">
                <?= $current_module->language->multi_actions->hide ?>
            </button>
            <button onclick="launch_multirecord_actions(this, 'reviewing')">
                <?= $current_module->language->multi_actions->review ?>
            </button>
            <button onclick="launch_multirecord_actions(this, 'published')">
                <?= $current_module->language->multi_actions->publish ?>
            </button>
        <? endif; ?>
    </div>
    
    <table id="posts_browser_table" class="nav_table evened">
        
        <thead>
        <tr>
            <th nowrap>
                <input type="checkbox" name="record_selections_trigger" onchange="toggle_record_selections(this)">
            </th>
            <? foreach( $browser->build_table_header($current_module->language->record_nav->columns) as $this_cell ): ?>
                <th align="<?= $this_cell->xalign ?>" <? if( $this_cell->xnowrap == true ) echo "nowrap "; ?>
                    <? if( ! empty($this_cell->xwidth) ) echo "width='$this_cell->xwidth' "; ?> class="<?= $this_cell->xclass ?>"><?
                    if( $this_cell->sort_asc->enabled == true ) echo "<img
                    src='{$config->full_root_path}/media/icons/order_asc" . ($order == $this_cell->sort_asc->order ? "_this" : "") . ".gif' 
                    align='absmiddle' width='7' height='7'  style='cursor: pointer;'
                    alt='{$this_cell->sort_asc->alt}' title='{$this_cell->sort_asc->alt}'
                    onclick=\"document.filter_form.order.value='{$this_cell->sort_asc->order}'; $('#filter_form').submit();\">&nbsp;";
                    ?><?= $this_cell->content ?><?
                    if( $this_cell->sort_desc->enabled == true ) echo "&nbsp;<img
                    src='{$config->full_root_path}/media/icons/order_desc" . ($order == $this_cell->sort_desc->order ? "_this" : "") . ".gif' 
                    align='absmiddle' width='7' height='7'  style='cursor: pointer;'
                    alt='{$this_cell->sort_desc->alt}' title='{$this_cell->sort_desc->alt}'
                    onclick=\"document.filter_form.order.value='{$this_cell->sort_desc->order}'; $('#filter_form').submit();\">";
                    ?></th>
            <? endforeach; ?>
        </tr>
        </thead>
        
        <tbody>
        <? foreach( $records as $row ): ?>
            <tr data-record-id="<?= $row->id_post ?>" class="<? if($row->status == "trashed") echo "deleted" ?>">
                <td nowrap>
                    <input type="checkbox" class="record_selector" name="record_selections[]" value="<?= $row->id_post ?>">
                </td>
                <td width="25%">
                    <div class="pseudo_link clipboard-copy" data-clipboard-text="<?= $row->id_post ?>">
                        <code>#<?= $row->id_post ?><span class="fa fa-copy fa-fw"></span></code>
                    </div>
                    
                    <div class="principal_data"><?= convert_emojis($row->title) ?></div>
                    
                    <div class="actions">
                        <? if( $account->level >= config::MODERATOR_USER_LEVEL || in_array($row->status, array("draft", "published", "reviewing")) ): ?>
                            <a class="action" href="<?= "{$config->full_root_url}/{$row->id_post}" ?>" target="_blank">
                                <span class="fa fa-external-link"></span>
                                <?= $language->view ?>
                            </a>
                        <? endif ?>
                        
                        <? if( $row->can_be_edited() && $row->status != "trashed" ): ?>
                            <span class="action pseudo_link" onclick="edit_post('<?= $row->id_post ?>')">
                                <span class="fa fa-pencil"></span>
                                <?= $current_module->language->record_nav->actions->edit ?>
                            </span>
                        <? endif ?>
                        
                        <? if( in_array($row->status, array("draft", "published", "reviewing")) ): ?>
                            <span class="action pseudo_link" onclick="copy_post('<?= $row->id_post ?>')">
                                <span class="fa fa-copy"></span>
                                <?= $current_module->language->record_nav->actions->copy ?>
                            </span>
                        <? endif; ?>
                        
                        <? if( $row->can_be_deleted() && $row->status != "trashed" ): ?>
                            <span class="action pseudo_link" onclick="change_post_status( '<?= $row->id_post ?>', 'trashed', this, function() { $('tr[data-record-id=<?= $row->id_post ?>]').fadeOut('fast'); } )">
                                <span class="fa fa-trash"></span>
                                <?= $current_module->language->record_nav->actions->trash ?>
                            </span>
                        <? endif; ?>
                        
                        <? if( $modules["messaging"]->enabled && $account->id_account != $row->id_author ): ?>
                            <span class="action pseudo_link" onclick="send_pm(this, '<?= $row->author_user_name ?>', '<?= htmlspecialchars($row->author_display_name) ?>')"
                                  title="<?= replace_escaped_vars( $language->contact->pm->title, '{$website_name}', $settings->get("engine.website_name") ) ?>">
                                <span class="fa fa-inbox"></span>
                                <?= $language->pm_author ?>
                            </span>
                        <? endif; ?>
                        
                        <? if( $account->level >= config::MODERATOR_USER_LEVEL && $row->status == "published" ): ?>
                            <span class="action important pseudo_link"
                                  onclick="change_post_status( '<?= $row->id_post ?>', 'reviewing', this, function() { $('tr[data-record-id=<?= $row->id_post ?>]').fadeOut('fast'); } )">
                                <span class="fa fa-flag"></span>
                                <?= $current_module->language->record_nav->actions->flag_for_review ?>
                            </span>
                        <? endif; ?>
                        
                        <? if( $account->level >= config::MODERATOR_USER_LEVEL && $row->status == "reviewing" ): ?>
                            <span class="action greengo pseudo_link"
                                  onclick="change_post_status( '<?= $row->id_post ?>', 'published', this, function() { $('tr[data-record-id=<?= $row->id_post ?>]').fadeOut('fast'); } )">
                                <span class="fa fa-check"></span>
                                <?= $current_module->language->record_nav->actions->publish ?>
                            </span>
                        <? endif; ?>
                        
                        <? if( $account->level >= config::MODERATOR_USER_LEVEL && $row->status == "trashed" ): ?>
                            <span class="action critical pseudo_link" onclick="untrash_for_review('<?= $row->id_post ?>')">
                                <span class="fa fa-reply"></span>
                                <?= $current_module->language->record_nav->actions->untrash_for_review ?>
                            </span>
                        <? endif; ?>
                        
                        <? if( $account->level >= config::MODERATOR_USER_LEVEL && in_array($row->status, array("trashed", "published", "reviewing")) ): ?>
                            <span class="action critical pseudo_link"
                                  onclick="change_post_status( '<?= $row->id_post ?>', 'draft', this, function() { $('tr[data-record-id=<?= $row->id_post ?>]').fadeOut('fast'); } )">
                                <span class="fa fa-reply"></span>
                                <?= $current_module->language->record_nav->actions->back_to_draft ?>
                            </span>
                        <? endif; ?>
                        
                    </div>
                </td>
                <td data-column="details">
                    
                    <?= empty($row->excerpt) ? "―" : convert_emojis(make_excerpt_of($row->excerpt, 100)) ?><br>
                    
                    <? render_expiration_date($row); ?>
                    
                    <span class="framed_content inlined"><?= $current_module->language->status_labels->{$row->status} ?></span>
                    
                    <?
                    render_post_metas($row);
                    
                    render_post_tags($row);
                    
                    if($account->level >= config::MODERATOR_USER_LEVEL) render_post_attached_media($row);
                    
                    if( ! empty($per_column_function_hooks["details"]) )
                        foreach($per_column_function_hooks["details"] as $function)
                            if( function_exists($function) ) $function($row);
                    ?>
                    
                </td>
                <td>
                    <a class="user_display_name nowrap" data-user-level="<?= $row->author_level ?>"
                        href="<?= "{$config->full_root_path}/user/{$row->author_user_name}" ?>">
                        <span class="fa fa-user fa-fw"></span><?= $row->get_processed_author_display_name() ?>
                    </a><br>
                    
                    <? if( $account->level >= config::MODERATOR_USER_LEVEL ): ?>
                        <a class="nowrap" href="<?= $config->full_root_path ?>/?s=ip:<?= $row->creation_ip ?>">
                            <span class="fa fa-cloud fa-fw"></span>
                            <?= $row->creation_ip ?>
                        </a><br>
                        
                        <?
                        $parts   = explode("; ", $row->creation_location);
                        $isp      = array_pop($parts);
                        $country  = array_pop($parts);
                        $city     = implode("; ", $parts);
                        if( $city != "N/A; N/A") : ?>
                            <a href="<?= $config->full_root_path ?>/?s=city:<?= urlencode($city) ?>">
                                <span class="fa fa-map-marker fa-fw"></span><?= $city ?>
                            </a><br>
                        <? endif; ?>
                        <? if($country != "N/A"): ?>
                            <a href="<?= $config->full_root_path ?>/?s=country:<?= urlencode($country) ?>">
                                <span class="fa fa-globe fa-fw"></span><?= $country ?>
                            </a><br>
                        <? endif; ?>
                        <? if($isp != "N/A"): ?>
                            <a href="<?= $config->full_root_path ?>/?s=isp:<?= urlencode($isp) ?>">
                                <span class="fa fa-building fa-fw"></span><?= $isp ?>
                            </a><br>
                        <? endif; ?>
                    <? endif; ?>
                </td>
                <td align="center">
                    <?= time_mini_string($row->creation_date) ?><br>
                    (<?= time_elapsed_string($row->creation_date) ?>)
                </td>
                <td align="center">
                    <? if( $row->publishing_date == "0000-00-00 00:00:00" ): ?>
                        &mdash;
                    <? else: ?>
                        <?= time_mini_string($row->publishing_date) ?><br>
                        (<?= time_elapsed_string($row->publishing_date) ?>)
                    <? endif; ?>
                </td>
                <td align="center">
                    <?= time_mini_string($row->last_update) ?><br>
                    (<?= time_elapsed_string($row->last_update) ?>)
                </td>
                <td align="right">
                    <?= $row->views ?>
                </td>
                <td align="right">
                    <?= $row->comments_count ?>
                </td>
                <td width="200">
                    <div class="thumbnail">
                        <? $src = empty($row->featured_image_thumbnail)
                            ? "{$config->full_root_path}/media/missing_image.png"
                            : $row->featured_image_thumbnail; ?>
                        <img src="<?= $src ?>">
                    </div>
                </td>
            </tr>
        <? endforeach; ?>
        </tbody>
        
    </table>
</div>

<div class="pagination">
    <? $browser->render_pagination_controls("paginate", $pagination); ?>
</div>

<?php

function render_expiration_date(post_record $row)
{
    global $current_module;
    
    if( $row->expiration_date != "0000-00-00 00:00:00")
    {
        if( date("Y-m-d H:i:s") <= $row->expiration_date )
            echo "<span class='framed_content inlined state_active'>" 
                . $current_module->language->record_nav->extras->expires . ": "
                . preg_replace('/\d+s/', "", time_remaining_string($row->expiration_date))
                . "</span><br>"
            ;
        else
            echo "<span class='framed_content inlined state_highlight'>"
                . $current_module->language->record_nav->extras->expired . ": "
                . time_elapsed_string($row->expiration_date)
                . "</span><br>"
            ;
    }
}

function render_post_metas(post_record $row)
{
    global $current_module, $repository;
    
    if( $row->pin_to_home )
        echo "
            <span class='framed_content inlined state_highlight'>
                <span class='fa fa-thumb-tack'></span>{$current_module->language->record_nav->pinned_to_home}
            </span>
        ";
    
    if( $row->pin_to_main_category_index )
        echo "
            <span class='framed_content inlined state_highlight'>
                <span class='fa fa-thumb-tack'></span><span class='category'><span 
                      class='fa fa-folder fa-fw'></span>{$row->main_category_title}</span>
            </span>
        ";
    else
        echo "<span class='category'><span class='fa fa-folder fa-fw'></span>{$row->main_category_title}</span> ";
    
    foreach($repository->get_attached_categories($row->id_post) as $category)
        echo "<span class='category'><span class='fa fa-folder fa-fw'></span>{$category->title}</span> ";
}

function render_post_tags(post_record $row)
{
    global $account, $current_module;
    
    $remove_tag_link = "";
    $caption         = str_replace('{$tag}', "%tag%", $current_module->language->record_nav->extras->remove_tag);
    if( $row->can_be_edited() ) $remove_tag_link = "
        <span class='pseudo_link fa fa-times fa-border' title='{$caption}'
              onclick='remove_tag_from_post($(this).closest(\".hashtag\"), \"{$row->id_post}\", \"%tag%\")'></span>
    ";
    if( $account->level < config::MODERATOR_USER_LEVEL )
    {
        foreach( $row->get_filtered_tags_list() as $tag)
        {
            echo "<span class='hashtag'><span class='fa fa-hashtag fa-fw'></span>{$tag}</span> ";
        }
    }
    else
    {
        if( is_array($row->tags_list) )
        {
            foreach( $row->tags_list as $tag)
            {
                $link = str_replace('%tag%', $tag, $remove_tag_link);
                echo "<span class='hashtag'><span class='fa fa-hashtag fa-fw'></span>{$tag}{$link}</span> ";
            }
        }
    }
}

function render_post_attached_media(post_record $row)
{
    global $media_repository, $config;
    
    if( empty($row->media_list) ) return;
        
    echo "\n<hr>\n";
    
    /*
    if( ! empty($row->id_featured_image) ):
        $featured_image = $media_repository->get($row->id_featured_image); ?>
        <!--
        <div class="media_bullet">
            <span class="bullet fa fa-picture-o fa-fw"></span>
            <a href="<?= $config->full_root_path ?>/user/<?= $featured_image->author_user_name ?>"
               class="user_display_name" data-user-level="<?= $featured_image->author_level ?>">
                <span class="fa fa-user fa-fw"></span>
                <?= $featured_image->get_processed_author_display_name() ?></a>
            [<?= time_elapsed_string($featured_image->publishing_date) ?>]
            <a class="prettyPhoto" href="<?= $featured_image->get_thumbnail_url() ?>"><?= $featured_image->title ?></a>
            <a href="<?= $featured_image->get_page_url() ?>" target="_blank"><span class="fa fa-external-link fa-fw"></span></a>
        </div>
        -->
        <?
    endif;
    */
    
    $media_records = $media_repository->get_multiple($row->media_list);
    foreach($media_records as $media_record):
        $icon = $media_record->type == "image" ? "fa-picture-o" : "fa-video-camera"; ?>
        <div class="media_bullet">
            <span class="bullet fa <?= $icon ?> fa-fw"></span>
            <a href="<?= $config->full_root_path ?>/user/<?= $media_record->author_user_name ?>"
               class="user_display_name" data-user-level="<?= $media_record->author_level ?>">
                <span class="fa fa-user fa-fw"></span>
                <?= $media_record->get_processed_author_display_name() ?></a>
            [<?= time_elapsed_string($media_record->publishing_date) ?>]
            <a class="prettyPhoto" href="<?= $media_record->get_thumbnail_url() ?>"><?= $media_record->title ?></a>
            <a href="<?= $media_record->get_page_url() ?>" target="_blank"><span class="fa fa-external-link fa-fw"></span></a>
        </div>
        <?
    endforeach;
}
