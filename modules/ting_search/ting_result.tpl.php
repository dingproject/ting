<div id="ting-facet-browser">
</div>
<div id="ting-search-sort">
  <?php print t('Sorted by'); ?>
  <select id="edit-ting-search-sort">
    <?php foreach ($sort_options as $sort => $label) { ?>
      <?php print '<option value="' . $sort . '">' . check_plain($label) . '</option>'; ?>
    <?php } ?>
  </select>
</div>
<div id="ting-search-summary">
  <?php print t('Showing !firstResult-!lastResult of !count results',
                array(
                  '!firstResult' => '<span class="firstResult"></span>',
                  '!lastResult' => '<span class="lastResult"></span>',
                  '!count' => '<span class="count"></span>',
                )); ?>
</div>
<div id="ting-search-legend">
  <ul>
    <li><span class="available marker">&#x25A0;</span>Hjemme</li>
    <li><span class="out marker">&#x25A0;</span>Udlånt</li>
  </ul>
</div>
<div id="ting-search-result">
	<ul>
	</ul>
</div>
