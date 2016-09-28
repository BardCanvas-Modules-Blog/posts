
function hook_abandon_post()
{
    $(window).bind('beforeunload.posts', function()
    {
        return abandon_post_question;
    });
}

function unhook_abandon_post()
{
    $(window).unbind('beforeunload.posts');
}

$(document).ready(function()
{
    $('#left_sidebar').find('.group.archive_tree .item.year').click(function()
    {
        var $parent = $(this).closest('.group');
        var year    = $(this).attr('data-year');
        
        $parent.find('.item[data-year="' + year + '"]').toggleClass('collapsed expanded');
    });
});

function remove_tag_from_post(trigger, id_post, tag)
{
    var $trigger = $(trigger);
    
    var url = $_FULL_ROOT_PATH
            + '/posts/scripts/remove_tag.php'
            + '?id_post=' + id_post
            + '&tag=' + tag
            + '&wasuuup=' + wasuuup()
        ;
    $trigger.block(blockUI_smallest_params);
    $.get(url, function(response)
    {
        $trigger.unblock();
        if( response != 'OK' )
        {
            alert(response);
            
            return;
        }
        
        $trigger.remove();
    });
}

function toggle_post_category_message(source_select)
{
    var $select  = $(source_select);
    var id       = $select.find('option:selected').val();
    var $targets = $select.closest('.field').find('.category_messages .message');
    
    if( $targets.length == 0 ) return;
    $targets.each(function()
    {
        var message_for = $(this).attr('data-category-id');
        if( message_for == id ) $(this).show();
        else                    $(this).hide();
    });
}

function add_attachment_to_quick_post(type, icon)
{
    var html  = '<div class="attachment framed_content state_highlight clearfix">'
        +     '<span class="fa pseudo_link fa-trash fa-fw pull-right" onclick="remove_attachment_from_quick_post(this)"></span>'
        +     sprintf('<span class="fa %s fa-fw"></span> ', icon)
        +     sprintf('<input type="file" name="attachments[%1$s][]" accept="%1$s/*"> ', type)
        + '</div>';
    
    $('#post_form').find('.field.attachments').append(html);
}

function remove_attachment_from_quick_post(trigger)
{
    $(trigger).closest('.attachment').fadeOut('fast', function() { $(this).remove(); });
}

function expand_quick_post_area()
{
    var $form = $('#post_form');
    var $trigger = $form.find('textarea[name="title"]');
    
    $form.find('.rest').show();
    $trigger.toggleClass('ready', true);
    $trigger.attr('placeholder', $trigger.attr('data-expanded-placeholder'));
    hook_abandon_post();
}

function change_post_status(id_post, new_status, trigger, callback)
{
    if( ! confirm($_GENERIC_CONFIRMATION) ) return;
    
    var url = $_FULL_ROOT_PATH + '/posts/scripts/toolbox.php';
    var params = {
        action:     'change_status',
        new_status: new_status,
        id_post:    id_post,
        wasuuup:    wasuuup()
    };
    
    $(trigger).block(blockUI_smallest_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert(response);
            $(trigger).unblock();
            
            return;
        }
        
        $(trigger).unblock();
        if( callback ) callback();
    });
}
