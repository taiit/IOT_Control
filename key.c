struct key
{
	char key_id;
	unsigned int time_stamp;
	unsigned int runing_time;
	char allocate_queue_time;
	char current_use_queue_time;
	unsigned int* queue_key_time;
};
struct key[MAX_KEY] local_key

ISR(timer_ovf) {
	for (int i = 0; i < MAX_KEY; ++i)
	{
		if(is_button_press(local_key[i].key_id)) {//key_press, counting the time
			local_key[i].runing_time++;
		} else {//key release, get key time_stamp
			local_key[i].time_stamp = local_key[i].runing_time;
			local_key[i].runing_time = 0;
		}
	}
}

void push_time_to_queue(unsigned int time, int idx) {
	//index should correct
	struct key push_key = local_key[idx];
	if (push_key.current_use_queue_time == push_key.allocate_queue_time) {
		push_key.queue_key_time = (unsigned int*) relloc(sizeof(unsigned int*) * (push_key.allocate_queue_time + 4))
		push_key.allocate_queue_time += 4
	}
	bool need_to_push = true;
	for (int i = 0; i < push_key.current_use_queue_time; ++i)
	{
		if(push_key.queue_key_time[i] == time) {
			need_to_push = false;
			break;
		}
	}
	if (need_to_push) {
		push_key.queue_key_time[push_key.current_use_queue_time] = time;
		push_key.current_use_queue_time++;
	}
}

unsigned int pop_time_to_queue(int idx) {
	// local_key[idx].queue_key_time[indx];
}

bool is_long_key_press(char key_id, unsigned int time) {
	for (int i = 0; i < MAX_KEY; ++i)
	{
		struct key check_key = local_key[i]
		if(key_id == check_key.key_id) {//match key
			if(check_key.time_stamp > 0) {//this is release now, let see how long does it hold

				//sort
				qsort();
				for (int i = 0; i < check_key.current_use_queue_time; ++i)
				{
					/* code */
					if(check_key.time_stamp > check_key.queue_key_time[i]) {
						delete[] check_key.queue_key_time;
						check_key.current_use_queue_time = 0;
						check_key.allocate_queue_time = 0;
						return true;
					}
				}
				return false;
				
			} else {
				push_time_to_queue(time, i);
			}
		}
	}

}
