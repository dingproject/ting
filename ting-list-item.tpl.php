<?php
// $Id$
/**
 * @file
 * Display a ting objects as part of a list.
 *
 * Available variables:
 * - $object: The thing..
 * - $local_id: The local id if the thing.
 * - $type: Type of the thing.
 * - $image: Image.
 * - $date: The date of the thing.
 * - $creator: Primary author.
 * - $additional_creators: Other authors.
 * - $language: The language of the item.
 * - $more_link: Link to details page.
 */
?>
<!-- ting-list-item.tpl -->
<div id="ting-item-<?php print $local_id; ?>" class="ting-item clearfix">

  <div class="content clearfix">
    <div class="picture">
      <?php if ($image) { ?>
        <?php print $image; ?>
      <?php } ?>
    </div>

    <div class="info">
      <span class='date'><?php print $date; ?></span>
      <h3><?php print $title; ?></h3>

      <em><?php echo t('by'); ?></em>
      <?php print $creator ?>

      <div class='language'><?php echo t('Language') . ': ' . $language; ?></div>
      <?php
      foreach ($additional_creators as $creator) {
        print "<p>" . $creator . "</p>";
      }
      ?>

      <div class="more">
        <?php print $more_link; ?>
      </div>
      <?php
        // TODO: This should go into ting_availability.
      if ($type != 'Netdokument') { ?>
      <div class="ting-status waiting">Afventer data…</div>
      <?php } ?>
    </div>

  </div>

  <?php if ($buttons) :?>
    <div class="ting-object-buttons">
      <?php print theme('item_list', $buttons, NULL, 'ul', array('class' => 'buttons')) ?>
    </div>
  <?php endif; ?>
</div>
