<?php
/**
 * @file ting_object.tpl.php
 *
 * Template to render objects from the Ting database.
 */
?>
<div id="ting-item-<?php print $ting_local_id; ?>" class="ting-item ting-item-full">
  <div class="ting-overview clearfix">
    <div class="left-column left">
      <div class="picture">
        <?php if ($image) { ?>
          <?php print $image; ?>
        <?php } ?>
      </div>

    </div>

    <div class="right-column left">
      <h2><?php print $ting_title; ?></h2>

      <div class='creator'>
        <?php if (sizeof($ting_creators_links) == 1) { ?>
          <span class='byline'><?php echo ucfirst(t('by')); ?></span>
          <?php print $ting_creators_links[0]; ?>
        <?php } ?>
        <?php if ($ting_publication_date) { ?>
          <span class='date'>(<?php print $ting_publication_date; ?>)</span>
        <?php } ?>
      </div>

      <?php if (isset($ting_title_full)) { ?>
        <p class="title-info">
           <span class="label"><?php print t('Additional title information:')?></span>
          <?php print $ting_title_full; ?>
        </p>
      <?php }?>

      <p class="abstract"><?php print $ting_abstract; ?></p>

      <?php if (isset($ting_series_links)) { ?>
        <p class="series">
         <span class="label"><?php print t('Series:')?></span>
          <?php print theme('item_list', $ting_series_links, NULL, 'span'); ?>
        </p>
      <?php } ?>

      <?php if (isset($additional_main_content)) { print drupal_render($additional_main_content); } ?>
    </div>

    <?php if ($buttons) :?>
      <div class="ting-object-buttons">
      <?php print theme('item_list', $buttons, NULL, 'ul', array('class' => 'buttons')) ?>
      </div>
    <?php endif; ?>

  </div>

  <div class="object-information ting-details clearfix">
    <?php print $ting_details; ?>
  </div>

  <?php
  $collection = ting_get_collection_by_id($object->id);
  if ($collection instanceof TingClientObjectCollection && is_array($collection->types)) {
    // Do we have more than only this one type?
    if (count($collection->types) > 1) {
      print '<div class="ding-box-wide object-otherversions">';
      print '<h3>'. t('Also available as: ') . '</h3>';
      print "<ul>";
      foreach ($collection->types as $type) {
        if ($type != $object->type) {
          $material_links[] = '<li class="category">' . l($type, $collection->url, array('fragment' => $type)). '</li>';
        }
      }
      print implode(' ', $material_links);
      print "</ul>";
      print "</div>";
    }
  }
  ?>

  <?php if (isset($additional_content)) { print drupal_render($additional_content); } ?>
</div>
