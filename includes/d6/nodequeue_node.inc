<?php

/**
 * Migrate nodequeue-node relationships.
 *
 * TODO Nodequeues are expected to exist in D7 prior to the migration (featurized).
 * Nodequeues mapping is based on the machine name -- not on the qid.
 * TODO remove $queues_map
 * TODO remove any refs
 */
class NodequeueNodeDrupal6Migration extends DrupalMigration {
  /**
   * Hard-coded map of D6 machine names => D7 machine names of nodequeues.
   */
  protected $queues_map = array('maps');
  public function __construct($args) {
    parent::__construct($args);

    // Declare source and destination objects.
    $options = array('fix_field_names' => array(), 'map_joinable' => FALSE);
    $this->source      = new MigrateDrupal6SourceSql($this->query(), array(), NULL, $options);
    $this->destination = new NodequeueNodeDestination();

    // Map source with destination. Assume table schema is the same in D6 and D7.
    $key_schema = $this->destination->getKeySchema();
    $this->map = new MigrateSQLMap($this->machineName, $key_schema, $key_schema);

    // Declare field mappings.
    $this->addSimpleMappings(array('qid', 'sqid', 'nid', 'position', 'timestamp'));
  }

  /**
   * @override
   *
   * Get {nodequeue_node} data.
   * Join with {nodequeue_queue} to have the machine name.
   */
  protected function query() {
    $query = Database::getConnection('default', 'd6')
      ->select('nodequeue_nodes', 'qn');
    $query->join('nodequeue_queue', 'q', 'qn.qid = q.qid');
    $query = $query
      ->fields('qn')
      ->fields('q', array('name'));
    return $query;
  }

  /**
   * @override
   *
   * Set D7 qid/sqid based on the queues' machine names.
   * No need to set D7 nid since they will be preserved.
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // Make sure we know the D7 queue to populate.
    if (!isset($this->queues_map[$row->name])) {
      $this->queueMessage(t('Unable to add node @nid in queue %d6name. No D7 queue matches the D6 queue. Is this queue listed in NodequeueNodeMigration ?', array('@nid' => $row->nid, '%d6name' => $row->name)), self::MESSAGE_ERROR);
      return FALSE;
    }

    $name_d7 = $this->queues_map[$row->name];

    // Make sure the queue actually exists in D7.
    $queue = db_select('nodequeue_queue', 'nq')
      ->fields('nq')
      ->condition('name', $name_d7)
      ->execute()
      ->fetchObject();
    if (empty($queue)) {
      $this->queueMessage(t('Unable to add node @nid in D7 queue %d7name: this queue does not exist. Is feature reverted ?', array('@nid' => $row->nid, '%d7name' => $name_d7)), self::MESSAGE_ERROR);
      return FALSE;
    }

    // Make sure the subqueue actually exists in D7.
    $subqueue = db_select('nodequeue_subqueue', 'sq')
      ->fields('sq')
      ->condition('qid',  $queue->qid)
      ->condition('sqid', $queue->qid)
      ->execute()
      ->fetchObject();
    if (empty($subqueue)) {
      $this->queueMessage(t('Unable to add node @nid in D7 queue %d7name: this queue does not have a subqueue. Is feature reverted ?', array('@nid' => $row->nid, '%d7name' => $name_d7)), self::MESSAGE_ERROR);
      return FALSE;
    }

    // Set D7 qid/sqid.
    $row->qid  = $queue->qid;
    $row->sqid = $subqueue->sqid;
  }
}
