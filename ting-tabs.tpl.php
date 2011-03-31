<?php
/**
 * @file ting_tabs.tpl.php
 *
 * Template to render tabs for Ting objects.
 *
 * Available variables:
 * - $tabs_labels: String, the tab labels.
 * - $tabs_content: Array, the tab content.
 */
?>
<!-- ting_tabs.tpl -->
<div <?php if ($wrapper_id) {print 'id="' . $wrapper_id. '"';} ?>>

  <div class="content-left">

    <div class="tab-navigation">

    <?php print $tabs_labels; ?>

    </div>

    <?php foreach ($tabs_content as $id => $content): ?>
    <div id="<?php print $id; ?>" class="tab-navigation-main">
      <div class="tab-navigation-main-inner">
        <?php print $content; ?>
      </div>
    </div>
    <?php endforeach; ?>
  </div>

</div>

